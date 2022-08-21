/**
 *  @brief
 *      FAT filesystem for Secure Digital Memory Card and Internal Flash.
 *
 *		Some file tools like GNU tools e.g. ls, pwd, cat.
 *		Do not expect real UNIX commands not even comparable to the Busybox
 *		commands. The UNIX like shell commands are parsing words. The parameters
 *		are parsed from the input stream till the end of line.
 *		These commands are not intended to use in other words, they are used
 *		in the interpreter mode.
 *  @file
 *      fs.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-06-03
 *  @remark
 *      Language: C, STM32CubeIDE GCC
 *  @copyright
 *      Peter Schmid, Switzerland
 *
 *      This project Mecrsip-Cube is free software: you can redistribute it
 *      and/or modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation, either version 3 of
 *      the License, or (at your option) any later version.
 *
 *      Mecrsip-Cube is distributed in the hope that it will be useful, but
 *      WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *      General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with Mecrsip-Cube. If not, see http://www.gnu.org/licenses/.
 */

// System include files
// ********************
#include "cmsis_os.h"
#include "time.h"
#include <string.h>
#include <ctype.h>
#include <fd.h>


// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "fs.h"
#include "sd.h"
#include "ff.h"
#include "rtc.h"
#include "block.h"
#include "myassert.h"


// Defines
// *******
#define LINE_LENGTH				256
#define	RAM_SHARED				(SRAM2B_BASE + 0x1000)	// 4 KiB used by fd
#define	SCRATCH_SIZE			0x1000					// 4 KiB scratch

// Private typedefs
// ****************


// Private function prototypes
// ***************************

// Global Variables
// ****************

const char FS_Version[] = "  * FatFs for internal flash and microSD - Generic FAT fs module  R0.12c (C) 2017 ChaN\n";

FATFS FatFs_FD;	/* Work area (filesystem object) for logical flash drive (0) */
FATFS FatFs_SD;	/* Work area (filesystem object) for logical SD drive (1) */
FILINFO fno;	/* File information */
DIR dj;			/* Directory object */

char path[255];
char pattern[20];
char line[300]; /* Line buffer */


// RTOS resources
// **************

static osMutexId_t FS_MutexID;
static const osMutexAttr_t FS_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control
		0U					// size for control block
};


// Hardware resources
// ******************
extern RTC_HandleTypeDef hrtc;


// Private Variables
// *****************
uint8_t 	*mkfs_scratch;

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the filesystem
 *  @return
 *      None
 */
void FS_init(void) {
	mkfs_scratch = (uint8_t *) RAM_SHARED;	// 4 KiB scratch area for mkfs
//	mkfs_scratch = pvPortMalloc(FD_PAGE_SIZE);
	FS_MutexID = osMutexNew(&FS_MutexAttr);
	ASSERT_fatal(FS_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());


	/* Gives a work area to the flash drive */
	f_mount(&FatFs_FD, "0:", 0);
	/* Gives a work area to the SD drive */
	f_mount(&FatFs_SD, "1:", 0);
}


/**
 *  @brief
 *      Interprets the content of the file.
 *  @warning
 *  	evaluate never returns if there is an error!
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @param[in]
 *      str   filename (w/ or w/o null termination)
 *  @param[in]
 *      count string length
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_include(uint64_t forth_stack, uint8_t *str, int count) {
	FIL fil;        /* File object */
	FRESULT fr;     /* FatFs return code */
	char *line;
	char *path;

	uint64_t stack;
	stack = forth_stack;

	line = (char *) pvPortMalloc(LINE_LENGTH);
	path = (char *) pvPortMalloc(LINE_LENGTH);

	memcpy(path, str, count);
	path[count] = 0;

	/* Open a text file */
	fr = f_open(&fil, path, FA_READ);
	if (fr) {
		// open failed
		stack = FS_type(stack, (uint8_t*)path, strlen(path));
		strcpy(line, ": can't find file\n");
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
	}

	/* Read every line and interprets it */
	while (f_gets(line, LINE_LENGTH-1, &fil)) {
		// line without \n
		// evaluate never returns if there is an error!
		stack = FS_evaluate(stack, (uint8_t*)line, strlen(line)-1);
	}

	/* Close the file */
	f_close(&fil);

	vPortFree(line);
	vPortFree(path);
	return stack;
}


/**
 *  @brief
 *      Dumps the flash memory (core) into a file.
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @param[in]
 *      str   filename (w/ or w/o null termination)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_coredump(uint64_t forth_stack, uint8_t *str, int count) {
	FIL fil;        /* File object */
	FRESULT fr;     /* FatFs return code */
	UINT bytes_written;
	uint8_t a_flag = FALSE;
	uint8_t param = FALSE;
	uint32_t size;

	uint64_t stack;
	stack = forth_stack;

	memcpy(path, str, count);
	path[count] = 0;
	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			if (!param) {
				path[0] = 0;
			}
			break;
		}
		memcpy(path, str, count);
		path[count] = 0;
		if (! strcmp (path, "-a")) {
			a_flag = TRUE;
		} else {
			param = TRUE;
		}
	}

	/* Open a file */
	fr = f_open(&fil, path, FA_CREATE_NEW | FA_WRITE);
	if (fr != FR_OK) {
		// open failed
		strcpy(line, "Err: can't open for write");
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
		return stack;
	}

	if (a_flag) {
		// dump everything
		size = 768 * 1024; // 768 KiB
	} else {
		// get the end of the flash dictionary
		size = ((uint32_t) ZweitDictionaryPointer ) - 0x08000000;
	}
	fr = f_write(&fil, (uint8_t *) 0x08000000, size, &bytes_written);
	if ( (fr != FR_OK) || (bytes_written < size ) ) {
		// write failed
		strcpy(line, "Err: write failed");
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
	}

	/* Close the file */
	f_close(&fil);

	return stack;
}


/**
 *  @brief
 *      Concatenate files and print on the standard output
 *
 *      The parameters are taken from the command line (Forth tokens)
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_cat(uint64_t forth_stack) {
	uint8_t *str = NULL;
	int count = 1;
	uint8_t n_flag = FALSE;
	uint8_t outfile_flag = FALSE;
	uint8_t input_flag = FALSE;
	uint8_t EOF_flag = FALSE;
	int line_num = 0;
	FIL fil_in;		/* File object */
	FIL fil_out;	/* File object */
	FRESULT fr;		/* FatFs return code */
	BYTE mode;

	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);

	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			// no more tokens
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;
		if (! strcmp(line, "-n")) {
			n_flag = TRUE;
		} else if ( (! strcmp(line, ">")) || (! strcmp(line, ">>")) ) {
			if (! strcmp(line, ">")) {
				// new file
				mode = FA_CREATE_ALWAYS | FA_WRITE;
			} else {
				// append
				mode = FA_OPEN_APPEND | FA_WRITE;
			}
			stack = FS_token(stack, &str, &count);
			memcpy(line, str, count);
			line[count] = 0;
			if (count == 0) {
				// no more tokens
				break;
			}
			outfile_flag = TRUE;
			fr = f_open(&fil_out, line, mode);
			if (fr != FR_OK) {
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": can't create file");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				break;
			}
		} else if (! strcmp(line, "<<") ) {
			stack = FS_token(stack, &str, &count);
			memcpy(pattern, str, count);
			line[count] = 0;
			if (count == 0) {
				// no more tokens
				break;
			}
			input_flag = TRUE;
		} else {
			/* Open a text file */
			fr = f_open(&fil_in, line, FA_READ);
			if (fr != FR_OK) {
				// open failed
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": file not found");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			} else {
				/* Read every line and type it */
				while (f_gets(line, sizeof(line), &fil_in)) {
					if (n_flag) {
						snprintf(pattern, sizeof(pattern), "%6i: ", line_num++);
						if (outfile_flag) {
							f_puts(pattern, &fil_out);
						} else {
							stack = FS_type(stack, (uint8_t*)pattern, strlen(pattern));
						}
					}
					if (outfile_flag) {
						f_puts(line, &fil_out);
					} else {
						stack = FS_type(stack, (uint8_t*)line, strlen(line));
					}
				}
				/* Close the file */
				f_close(&fil_in);
			}
		}
	}

	if (input_flag) {
		// input from console
		while (! EOF_flag) {
			// read till end of line
			count = 255;
			stack = FS_accept(stack, (uint8_t*)line, &count);
			line[count] = 0;
			if (! strcmp(line, pattern)) {
				// EOF
				EOF_flag = TRUE;
			} else {
				if (outfile_flag) {
					f_puts(line, &fil_out);
					f_putc('\n', &fil_out);
				} else {
					stack = FS_type(stack, (uint8_t*)line, strlen(line));
				}
			}
			stack = FS_cr(stack);
		}
	}

	if (outfile_flag) {
		f_close(&fil_out);
	}

	return stack;
}


/**
 *  @brief
 *      List directory contents.
 *
 *      The parameters are taken from the command line (Forth tokens)
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_ls(uint64_t forth_stack) {
	char attrib[6];
	uint8_t *str = NULL;
	int count = 1;
	uint8_t a_flag = FALSE;
	uint8_t l_flag = FALSE;
	uint8_t one_flag = FALSE;
	uint8_t param = FALSE;
	uint8_t column = 0;
	FRESULT fr;     /* FatFs return code */

	uint64_t stack;
	stack = forth_stack;

	memset(&dj, 0, sizeof(dj));
	memset(&fno, 0, sizeof(fno));


	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			if (!param) {
				line[0] = 0;
			}
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;
		if (! strcmp (line, "-a")) {
			a_flag = TRUE;
		} else if (! strcmp (line, "-l")) {
			l_flag = TRUE;
		} else if (! strcmp (line, "-1")) {
			one_flag = TRUE;
		} else {
			param = TRUE;
		}
	}

	if (strchr(line, '*') != NULL || strchr(line, '?') != NULL) {
		// there is a matching pattern string
		if (strrchr(line, '/') == NULL) {
			// no path, only pattern
			strncpy(pattern, line, sizeof(pattern));
			strcpy(path, "");
		} else {
			// path and pattern
			strncpy(pattern, strrchr(line, '/')+1, sizeof(pattern));
			strncpy(path, line, sizeof(path));
			path[strlen(line) - strlen(pattern) - 1] = 0;
		}
	} else {
		// only pathpattern
		strcpy(pattern, "*");
		strncpy(path, line, sizeof(path));
	}

	fr = f_findfirst(&dj, &fno, path, pattern);

	stack = FS_cr(stack);
	while (fr == FR_OK && fno.fname[0]) {
		/* Repeat while an item is found */
		if (l_flag) {
			strcpy(attrib, "----"); // drwa
			if ( (fno.fattrib & AM_DIR) == AM_DIR) {
				attrib[0] = 'd';
			}
			if ( (fno.fattrib & AM_SYS) != AM_SYS) {
				attrib[1] = 'r';
			}
			if ( (fno.fattrib & AM_RDO) != AM_RDO &&
				 (fno.fattrib & AM_SYS) != AM_SYS ) {
				attrib[2] = 'w';
			}
			if ( (fno.fattrib & AM_ARC) == AM_ARC) {
				attrib[3] = 'a';
			}

			snprintf(line, sizeof(line), "%s %9u %4u-%02u-%02uT%02u:%02u:%02u %s\n",
					attrib,
					(unsigned int)fno.fsize,
					(fno.fdate >> 9) + 1980,  (fno.fdate >> 5) & 0xF,  fno.fdate & 0x1F,
					(fno.ftime >> 11) & 0x1F, (fno.ftime >> 5) & 0x2F, (fno.ftime & 0x1F)*2,
					fno.fname);
		} else {
			// not long format
			if (one_flag) {
				// one column
				snprintf(line, sizeof(line), "%s\n", fno.fname);
			} else {
				// 4 columns
				snprintf(line, sizeof(line), "%-23s ", fno.fname);
				if ( ( (fno.fattrib & AM_HID) != AM_HID) || a_flag) {
					if ( (++column) >= 4) {
						strncat(line, "\n", sizeof(line)-1);
						column = 0;
					}
				}
			}
		}
		if ( ( (fno.fattrib & AM_HID) != AM_HID) || a_flag) {
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
		}
		/* Search for next item */
		fr = f_findnext(&dj, &fno);
	}
	if (!l_flag && column != 0) {
		stack = FS_cr(stack);
	}

	f_closedir(&dj);

	return stack;
}

/**
 *  @brief
 *      Change the working directory.
 *
 *      The parameters are taken from the command line (Forth tokens)
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_cd(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	uint8_t *str = NULL;
	int count = 1;

	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);

	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;

		fr = f_chdir(line);
		if (fr != FR_OK) {
			strcpy(line, "Err: directory not found");
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
			break;
		}
	}

	return stack;
}


/**
 *  @brief
 *      Print working directory
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_pwd(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */

	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);
	fr = f_getcwd(line, sizeof(line));  /* Get current directory path */
	if (fr == FR_OK) {
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
	} else {
		strcpy(line, "Err: no working directory");
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
	}

	return stack;
}


/**
 *  @brief
 *      Make directories
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_mkdir(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	uint8_t *str = NULL;
	int count = 1;

	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);

	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			// no more tokens
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;

		fr = f_mkdir(line);  /* create directory */
		if (fr != FR_OK) {
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
			strcpy(line, ": can't create directory ");
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
		}
	}

	return stack;
}


/**
 *  @brief
 *      Remove files or directories
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_rm(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	uint8_t *str = NULL;
	int count = 1;

	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);

	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			// no more tokens
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;

		fr = f_unlink(line);  /* remove file or directory */
		if (fr != FR_OK) {
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
			strcpy(line, ": can't remove file or directory  ");
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
		}
	}

	return stack;
}


/**
 *  @brief
 *      Change file mode bits
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_chmod(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	uint8_t *str = NULL;
	int count = 1;
	uint8_t param = FALSE;

	uint64_t stack;
	stack = forth_stack;
	BYTE attr = 0;
	BYTE mask = 0;

	stack = FS_cr(stack);

	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			if (!param) {
				line[0] = 0;
			}
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;
		if (! strcmp (line, "=r")) {
			attr = AM_RDO;
			mask = AM_RDO | AM_SYS | AM_HID | AM_RDO;
		} else if (! strcmp (line, "=w")) {
			attr = AM_SYS | AM_HID;
			mask = AM_ARC | AM_RDO | AM_HID | AM_SYS ;
		} else if (! strcmp (line, "=a")) {
			attr = AM_ARC | AM_SYS | AM_HID | AM_RDO;
			mask = AM_ARC | AM_RDO | AM_HID | AM_SYS ;
		} else if (! strcmp (line, "=rw")) {
			attr = 0;
			mask = AM_ARC | AM_RDO | AM_HID | AM_SYS ;
		} else if (! strcmp (line, "=ra")) {
			attr = AM_RDO | AM_ARC;
			mask = AM_ARC | AM_RDO | AM_HID | AM_SYS ;
		} else if (! strcmp (line, "=wa")) {
			attr = AM_SYS | AM_ARC;
			mask = AM_ARC | AM_RDO | AM_HID | AM_SYS ;
		} else if (! strcmp (line, "=rwa")) {
			attr = AM_ARC;
			mask = AM_ARC | AM_RDO | AM_HID | AM_SYS ;
		} else if (! strcmp (line, "=")) {
			attr = AM_SYS | AM_HID | AM_RDO;
			mask = AM_ARC | AM_RDO | AM_HID | AM_SYS ;
		} else {
			param = TRUE;
		}
	}

	fr = f_chmod(line, attr, mask);  /* remove file or directory */
	if (fr != FR_OK) {
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
		strcpy(line, ": can't change mode");
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
	}

	return stack;
}


/**
 *  @brief
 *      Change file timestamps or create files
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_touch(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	FIL fil;        /* File object */
	uint8_t *str = NULL;
	int count = 1;
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	uint64_t stack;
	stack = forth_stack;

	memset(&fno, 0, sizeof(fno));

	stack = FS_cr(stack);

	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			// no more tokens
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		fno.fdate = (WORD)(
				((sDate.Year + 20) << 9) |
				sDate.Month << 5 |
				sDate.Date);;
		fno.ftime = (WORD)(
				sTime.Hours << 11 |
				sTime.Minutes << 5 |
				sTime.Seconds / 2U);

		// check for file existence
		if (f_stat(line, NULL) == FR_NO_FILE) {
			// file does not exist -> create
			fr = f_open(&fil, line, FA_CREATE_NEW | FA_WRITE);
			if (fr == FR_OK) {
				fr = f_close(&fil);
			} else {
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": can't create file");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
		} else {
			// file exists
			fr = f_utime(line, &fno);  /* create directory */
			if (fr != FR_OK) {
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": can't update timestamps ");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
		}
	}

	return stack;
}


/**
 *  @brief
 *      Move (rename) files
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_mv(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	uint8_t *str = NULL;
	int count = 1;
	int param = 0;

	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);

	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			// no more tokens
			break;
		}

		param++;
		if (param == 1) {
			memcpy(path, str, count);
			path[count] = 0;
			continue;
		} else if (param == 2) {
			memcpy(line, str, count);
			line[count] = 0;
		} else {
			;
		}

	}

	if (param == 2) {
		fr = f_rename (path, line);  /* move file or directory */
		if (fr != FR_OK) {
			stack = FS_type(stack, (uint8_t*)path, strlen(path));
			strcpy(path, ": can't move/rename file or directory  ");
			stack = FS_type(stack, (uint8_t*)path, strlen(path));
		}
	} else {
		strcpy(path, "Wrong number of parameters");
		stack = FS_type(stack, (uint8_t*)path, strlen(path));
	}

	return stack;
}


/**
 *  @brief
 *      Copy files
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_cp(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	uint8_t *str = NULL;
	int count = 1;
	UINT rd_count, wr_count;
	int param = 0;
	FIL fil_src;	/* File object */
	FIL fil_dest;	/* File object */

	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);

	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			// no more tokens
			break;
		}

		param++;
		if (param == 1) {
			memcpy(path, str, count);
			path[count] = 0;
			continue;
		} else if (param == 2) {
			memcpy(line, str, count);
			line[count] = 0;
		} else {
			;
		}

	}

	if (param == 2) {
		fr = f_open(&fil_src, path, FA_READ);
		if (fr == FR_OK) {
			fr = f_open(&fil_dest, line, FA_CREATE_ALWAYS | FA_WRITE);
			if (fr == FR_OK) {
				// copy the file
				while (!f_eof(&fil_src)) {
					fr = f_read(&fil_src, mkfs_scratch, SCRATCH_SIZE, &rd_count);
					if (fr != FR_OK) {
						strcpy(path, "Read error");
						stack = FS_type(stack, (uint8_t*)path, strlen(path));
						break;
					}
					fr = f_write(&fil_dest, mkfs_scratch, rd_count, &wr_count);
					if (fr != FR_OK || rd_count != wr_count) {
						strcpy(path, "Write error");
						stack = FS_type(stack, (uint8_t*)path, strlen(path));
						break;
					}
				}
				f_close(&fil_src);
				f_close(&fil_dest);
			} else {
				// open destination failed
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": can't create file");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				f_close(&fil_src);
			}
		} else {
			// open source failed
			stack = FS_type(stack, (uint8_t*)path, strlen(path));
			strcpy(path, ": file not found");
			stack = FS_type(stack, (uint8_t*)path, strlen(path));
		}
	} else {
		strcpy(path, "Wrong number of parameters");
		stack = FS_type(stack, (uint8_t*)path, strlen(path));
	}

	return stack;
}


/**
 *  @brief
 *      Split a file into pieces
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_split(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	TCHAR* buf;
	uint8_t *str = NULL;
	int count = 1;
	uint8_t param = 0;
	FIL fil_src;	/* File object */
	FIL fil_dest;	/* File object */
	int lines = 1000;
	char letter='a';
	int line_count;
	int wr_count;

	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);

	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;
		if (! strcmp (line, "-l")) {
			// set lines
			stack = FS_token(stack, &str, &count);
			memcpy(line, str, count);
			line[count] = 0;
			if (count == 0) {
				// no more tokens
				break;
			}
			lines = atoi(line);
		} else {
			param++;
		}
	}

	if (param == 1) {
		strcpy(path, "xa");
		fr = f_open(&fil_src, line, FA_READ);
		if (fr == FR_OK) {
			// split the file
			while (!f_eof(&fil_src)) {
				path[1] = letter;
				fr = f_open(&fil_dest, path, FA_CREATE_ALWAYS | FA_WRITE);
				if (fr == FR_OK) {
					for (line_count = 0; line_count < lines; line_count++) {
						buf = f_gets(line, sizeof(line), &fil_src);
						if (*buf == NULL) {
							// no lines left
							break;
						}
						wr_count = f_puts(line, &fil_dest);
						if (wr_count < 0) {
							strcpy(line, "Write error");
							stack = FS_type(stack, (uint8_t*)line, strlen(line));
							break;
						}
					}
					f_close(&fil_dest);
				} else {
					// open destination file failed
					stack = FS_type(stack, (uint8_t*)path, strlen(path));
					strcpy(path, ": can't create file");
					stack = FS_type(stack, (uint8_t*)path, strlen(path));
					f_close(&fil_src);
					break;
				}
				if (letter < 'z') {
					letter++;
				} else {
					strcpy(path, "Too many files");
					stack = FS_type(stack, (uint8_t*)line, strlen(line));
					break;
				}
			}
			f_close(&fil_src);
			f_close(&fil_dest);
		} else {
			// open source file failed
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
			strcpy(line, ": file not found");
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
		}
	} else {
		strcpy(path, "Wrong number of parameters");
		stack = FS_type(stack, (uint8_t*)path, strlen(path));
	}

	return stack;
}


int count_words(const char *s) {
	int i, w;

	for (i = 0, w = 0; i < strlen(s); i++) {
		if (!isspace ((int) *(s+i))) {
			w++;
			while (!isspace ((int) *(s+i)) && *(s+i) != '\0') {
				i++;
			}
		}
	}

	return w;
}


/**
 *  @brief
 *      Word count, print newline, word, and byte counts for each file
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_wc(uint64_t forth_stack) {
	FIL fil;        /* File object */
	FRESULT fr;     /* FatFs return code */
	TCHAR* buf;
	uint8_t *str = NULL;
	int count = 1;
	unsigned int line_count = 0;
	unsigned int word_count = 0;
	unsigned int char_count = 0;

	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);

	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			break;
		}
		memcpy(path, str, count);
		path[count] = 0;

		fr = f_open(&fil, path, FA_READ);
		if (fr == FR_OK) {
			while (!f_eof(&fil)) {
				buf = f_gets(line, sizeof(line), &fil);
				if (*buf == NULL) {
					// no lines left
					break;
				}
				line_count++;
				word_count += count_words(line);
				char_count += strlen(line);
			}
			f_close(&fil);
			snprintf(line, sizeof(line), "%5u %5u %5u %s",
					line_count, word_count, char_count, path);
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
			stack = FS_cr(stack);
			line_count = 0;
			word_count = 0;
			char_count = 0;
		} else {
			// open file failed
			stack = FS_type(stack, (uint8_t*)path, strlen(path));
			strcpy(path, ": file not found");
			stack = FS_type(stack, (uint8_t*)path, strlen(path));
			break;
		}
	}

	return stack;
}


/**
 *  @brief
 *      Report file system disk space usage (1 KiB blocks)
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_df(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	FATFS *fatfs;
	DWORD nclst;

	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);
	fr = f_getfree("0:", &nclst, &fatfs);  /* Get current directory path */
	if (fr == FR_OK) {
		snprintf(line, sizeof(line), "0: %lu KiB (%lu clusters) total %lu KiB\n",
				nclst * (fatfs->csize)/2, nclst, (fatfs->n_fatent - 2) * (fatfs->csize)/2);
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
	} else {
		strcpy(line, "Err: no volume\n");
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
	}

	fr = f_getfree("1:", &nclst, &fatfs);  /* Get current directory path */
	if (fr == FR_OK) {
		snprintf(line, sizeof(line), "1: %lu KiB (%lu clusters) total %lu KiB",
				nclst * (fatfs->csize)/2, nclst, (fatfs->n_fatent - 2) * (fatfs->csize)/2);
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
	} else {
		strcpy(line, "Err: no volume");
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
	}

	return stack;
}


/**
 *  @brief
 *      Print or set time and time
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_date(uint64_t forth_stack) {
	uint8_t *str = NULL;
	int count = 1;
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	struct tm tm_s;
	uint8_t param = FALSE;

	uint64_t stack;
	stack = forth_stack;

	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			if (!param) {
				line[0] = 0;
			}
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;
	}

	stack = FS_cr(stack);

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);


	tm_s.tm_year = sDate.Year + 100;
	tm_s.tm_mon = sDate.Month - 1;
	tm_s.tm_mday = sDate.Date;

	tm_s.tm_hour = sTime.Hours;
	tm_s.tm_min = sTime.Minutes;
	tm_s.tm_sec = sTime.Seconds;

	tm_s.tm_isdst = 0;
	tm_s.tm_wday = sDate.WeekDay;
	tm_s.tm_yday = 0;

	strftime(line, sizeof(line), "%c", &tm_s);

	stack = FS_type(stack, (uint8_t*)line, strlen(line));
	stack = FS_cr(stack);

	return stack;
}


/**
 *  @brief
 *      Mount drive
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_mount(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	uint8_t *str = NULL;
	int count = 1;
	uint8_t param = FALSE;
	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);
	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			// no more tokens
			if (!param) {
				strcpy(line, "Specify drive 0:|1:|FLASH:|SD:");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;

		param = TRUE;
		if (! strcmp (line, "0:") || ! strcmp (line, "FLASH:")) {
			FD_getSize();
			fr = f_mount(&FatFs_FD, "0:", 0);
			if (fr != FR_OK) {
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": Can't mount drive");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
		} else if (! strcmp (line, "1:") || ! strcmp (line, "SD:")){
			SD_getSize();
			fr = f_mount(&FatFs_SD, "1:", 0);
			if (fr != FR_OK) {
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": Can't mount drive");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
		} else {
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
			strcpy(line, ": unknown drive");
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
		}

	}

	return stack;
}


/**
 *  @brief
 *      Umount the default drive
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_umount(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	uint8_t *str = NULL;
	int count = 1;
	uint8_t param = FALSE;
	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);
	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			// no more tokens
			if (!param) {
				strcpy(line, "Specify drive 0:|1:|FD:|SD:");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;

		param = TRUE;
		if (! strcmp (line, "0:") || ! strcmp (line, "FD:")) {
			FD_getSize();
			fr = f_mount(0, "0:", 0);
			if (fr != FR_OK) {
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": Can't unmount drive");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
		} else if (! strcmp (line, "1:") || ! strcmp (line, "SD:")){
			SD_getSize();
			fr = f_mount(0, "1:", 0);
			if (fr != FR_OK) {
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": Can't unmount drive");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
		} else {
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
			strcpy(line, ": unknown drive");
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
		}

	}

	return stack;
}


/**
 *  @brief
 *      Changes the current drive.
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_chdrv(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	uint8_t *str = NULL;
	int count = 1;
	uint8_t param = FALSE;
	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);
	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			// no more tokens
			if (!param) {
				strcpy(line, "Specify drive 0:|1:|FD:|SD:");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;

		param = TRUE;
		if (! strcmp (line, "0:") || ! strcmp (line, "FD:")) {
			fr = f_chdrive("0:");
			if (fr != FR_OK) {
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": invalid drive");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
		} else if (! strcmp (line, "1:") || ! strcmp (line, "SD:")){
			fr = f_chdrive("1:");
			if (fr != FR_OK) {
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": invalid drive");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
		} else {
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
			strcpy(line, ": invalid drive");
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
		}

	}

	return stack;
}


/**
 *  @brief
 *      Creates an FAT/exFAT volume on the logical drive.
 *
 *      mkfs [-t fat|fat32|extfat] 0:|1:
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_mkfs(uint64_t forth_stack) {
	FRESULT fr = FR_OK;     /* FatFs return code */
	uint8_t *str = NULL;
	int count = 1;
	uint8_t param = FALSE;
	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);
	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			// no more tokens
			if (!param) {
				strcpy(line, "Specify drive 0:|1:|FD:|SD:");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
			break;
		}
		memcpy(line, str, count);
		line[count] = 0;

		param = TRUE;
		if (! strcmp (line, "0:") || ! strcmp (line, "FD:")) {
			fr = f_mkfs("0:", FM_ANY, 0, mkfs_scratch, SCRATCH_SIZE);
		} else if (! strcmp (line, "1:") || ! strcmp (line, "SD:")){
			fr = f_mkfs("1:", FM_ANY, 0, mkfs_scratch, SCRATCH_SIZE);
		} else {
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
			strcpy(line, " invalid drive");
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
		}
		if (fr != FR_OK) {
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
			strcpy(line, " can't create filesystem.");
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
		}
	}

	return stack;
}


/**
 *  @brief
 *      Convert and copy files (Disk Destroyer :-)
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t FS_dd(uint64_t forth_stack) {
	FRESULT fr;     /* FatFs return code */
	uint8_t *str = NULL;
	int count = 1;
	UINT rd_count, wr_count;
	int param = 0;
	FIL fil_src;	/* File object */
	FIL fil_dest;	/* File object */
	int blocks_1k;
	int block;
	uint8_t status;

	uint64_t stack;
	stack = forth_stack;

	stack = FS_cr(stack);

	FD_getSize();
	blocks_1k = FD_getBlocks();
	while (TRUE) {
		// get tokens till end of line
		stack = FS_token(stack, &str, &count);
		if (count == 0) {
			// no more tokens
			break;
		}

		param++;
		if (param == 1) {
			memcpy(path, str, count);
			path[count] = 0;
			continue;
		} else if (param == 2) {
			memcpy(line, str, count);
			line[count] = 0;
		} else {
			;
		}
	}

	if (param == 2) {
		if (! strcmp (path, "0:")) {
			// flash drive is source, file is destination
			fr = f_open(&fil_dest, line, FA_CREATE_ALWAYS | FA_WRITE);
			if (fr == FR_OK) {
				// copy drive to file
				block = 0;
				for (block=0; block<(blocks_1k*2); block+=8) {
					status = FD_ReadBlocks(mkfs_scratch, block, 8);
					if (status != SD_OK) {
						strcpy(path, "\nRead error");
						stack = FS_type(stack, (uint8_t*)path, strlen(path));
						break;
					}
					fr = f_write(&fil_dest, mkfs_scratch, SCRATCH_SIZE, &wr_count);
					if (fr != FR_OK) {
						strcpy(path, "\nWrite error");
						stack = FS_type(stack, (uint8_t*)path, strlen(path));
						break;
					}
					sprintf(path, "%i KiB written \r", (block+8)/2);
					stack = FS_type(stack, (uint8_t*)path, strlen(path));
				}
				strcpy(path, "\n");
				stack = FS_type(stack, (uint8_t*)path, strlen(path));
				f_close(&fil_dest);
			} else {
				// open destination failed
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
				strcpy(line, ": can't create file");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}


		} else if (! strcmp (line, "0:")) {
			// flash drive is destination, file is source
			fr = f_open(&fil_src, path, FA_READ);
			if (fr == FR_OK) {
				// copy file to drive
				block = 0;
				while (!f_eof(&fil_src)) {
					fr = f_read(&fil_src, mkfs_scratch, SCRATCH_SIZE, &rd_count);
					if (fr != FR_OK) {
						strcpy(path, "\nRead error");
						stack = FS_type(stack, (uint8_t*)path, strlen(path));
						break;
					}
					if (rd_count < SCRATCH_SIZE) {
						// not enough read
						break;
					}
					if (block >= (blocks_1k*2)) {
						// no more blocks on destination
						break;
					}
					status = FD_WriteBlocks(mkfs_scratch, block, 8);
					if (status != SD_OK) {
						strcpy(path, "\nWrite error");
						stack = FS_type(stack, (uint8_t*)path, strlen(path));
						break;
					}
					block += 8;
					sprintf(path, "%i KiB written \r", block/2);
					stack = FS_type(stack, (uint8_t*)path, strlen(path));
				}
				strcpy(path, "\n");
				stack = FS_type(stack, (uint8_t*)path, strlen(path));
				f_close(&fil_src);
			} else {
				// open source failed
				stack = FS_type(stack, (uint8_t*)path, strlen(path));
				strcpy(path, ": file not found");
				stack = FS_type(stack, (uint8_t*)path, strlen(path));
			}

		} else {
			strcpy(path, "Source or destination has to be flash drive 0:");
			stack = FS_type(stack, (uint8_t*)path, strlen(path));
		}
	} else {
		strcpy(path, "Wrong number of parameters");
		stack = FS_type(stack, (uint8_t*)path, strlen(path));
	}

	return stack;
}


int FS_FIL_size(void) {
	return(sizeof(FIL));
}

int FS_FATFS_size(void) {
	return(sizeof(FATFS));
}

int FS_DIR_size(void) {
	return(sizeof(DIR));
}

int FS_FILINFO_size(void) {
	return(sizeof(FILINFO));
}

int FS_FILINFO_fsize(void) {
	return(0);
}

int FS_FILINFO_fdate(void) {
	return(sizeof(FSIZE_t));
}

int FS_FILINFO_ftime(void) {
	return(sizeof(FSIZE_t)+sizeof(WORD));
}

int FS_FILINFO_fattrib(void) {
	return(sizeof(FSIZE_t)+sizeof(WORD)+sizeof(WORD));
}

int FS_FILINFO_fname(void) {
#if _USE_LFN != 0
	return(sizeof(FSIZE_t)+sizeof(WORD)+sizeof(WORD)+sizeof(BYTE)+13*sizeof(BYTE));
#else
	return(sizeof(FSIZE_t)+sizeof(WORD)+sizeof(WORD)+sizeof(BYTE));
#endif
}

int FS_FILINFO_altname(void) {
	return(sizeof(FSIZE_t)+sizeof(WORD)+sizeof(WORD)+sizeof(BYTE));
}

int FS_f_eof(FIL* fp) {
	return f_eof(fp);
}

FSIZE_t FS_f_size(FIL* fp) {
	return f_size(fp);
}

int FS_f_error(FIL* fp) {
	return f_error(fp);
}

/**
 *  @brief
 *     Reads the next character from stream and returns it as an unsigned char cast to an
       int, or EOF on end of file or error.
 *  @param[in]
 *      fp   File object structure
 *  @return
 *     unsigned char cast to an int, or EOF on end of file or error.
 */
int FS_getc(FIL* fp) {
	int buffer;
	unsigned int count;

	if (f_write(fp, &buffer, 1, &count) != FR_OK) {
		return -1;
	}

	if (count != 1) {
		return -2;
	}
	return buffer;
}

// Private Functions
// *****************


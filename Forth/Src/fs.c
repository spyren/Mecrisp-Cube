/**
 *  @brief
 *      FAT filesystem for Secure Digital Memory Card.
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

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "fs.h"
#include "sd.h"
#include "ff.h"
#include "rtc.h"


// Defines
// *******


// Private typedefs
// ****************


// Private function prototypes
// ***************************

// Global Variables
// ****************
FATFS FatFs;	/* Work area (filesystem object) for logical drive */
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
		NULL,				// memory for control block
		0U					// size for control block
};


// Hardware resources
// ******************
extern RTC_HandleTypeDef hrtc;


// Private Variables
// *****************


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the filesystem
 *  @return
 *      None
 */
void FS_init(void) {
	FS_MutexID = osMutexNew(&FS_MutexAttr);
	if (FS_MutexID == NULL) {
		Error_Handler();
	}

	/* Gives a work area to the default drive */
	f_mount(&FatFs, "", 0);
}


/**
 *  @brief
 *      Interprets the content of the file.
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

	uint64_t stack;
	stack = forth_stack;

	memcpy(path, str, count);
	line[count] = 0;

	/* Open a text file */
	fr = f_open(&fil, path, FA_READ);
	if (fr) {
		// open failed
		strcpy(line, "Err: file not found");
		stack = FS_type(stack, (uint8_t*)line, strlen(line));
	}

	/* Read every line and interprets it */
	while (f_gets(line, sizeof line, &fil)) {
//		// type the line
//		FS_type((uint8_t*)line, strlen(line));
//		osDelay(100);
		// line without \n
		stack = FS_evaluate(stack, (uint8_t*)line, strlen(line)-1);
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
	int line_num = 0;
	FIL fil;        /* File object */
	FRESULT fr;     /* FatFs return code */

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
		} else {
			/* Open a text file */
			fr = f_open(&fil, line, FA_READ);
			if (fr) {
				// open failed
				strcpy(line, "Err: file not found");
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}
			/* Read every line and type it */
			while (f_gets(line, sizeof(line), &fil)) {
				if (n_flag) {
					snprintf(pattern, sizeof(pattern), "%6i: ", line_num++);
					stack = FS_type(stack, (uint8_t*)pattern, strlen(pattern));
				}
				stack = FS_type(stack, (uint8_t*)line, strlen(line));
			}

			/* Close the file */
			f_close(&fil);

		}
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
						strncat(line, "\n", sizeof(line));
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
	uint8_t *str = NULL;
	int count = 1;
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

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

		fr = f_utime(line, &fno);  /* create directory */
		if (fr != FR_OK) {
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
			strcpy(line, ": can't update timestamps ");
			stack = FS_type(stack, (uint8_t*)line, strlen(line));
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
	fr = f_getfree("", &nclst, &fatfs);  /* Get current directory path */
	if (fr == FR_OK) {
		snprintf(line, sizeof(line), "%lu KiB (%lu SD-Blocks)", nclst/2, nclst);
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
	uint8_t s_flag = FALSE;
	uint8_t i_flag = FALSE;

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
		if (! strcmp (line, "-s")) {
			s_flag = TRUE;
		} else if (! strcmp (line, "-i")) {
			i_flag = TRUE;
		} else {
			param = TRUE;
		}

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

	strftime(line, sizeof(line), "%c", &tm_s);

	stack = FS_type(stack, (uint8_t*)line, strlen(line));
	stack = FS_cr(stack);

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



// Private Functions
// *****************


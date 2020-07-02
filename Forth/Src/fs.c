/**
 *  @brief
 *      FAT filesystem for Secure Digital Memory Card.
 *
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
#include <string.h>

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "fs.h"
#include "sd.h"
#include "ff.h"


// Defines
// *******


// Private typedefs
// ****************


// Private function prototypes
// ***************************
void FORTH_evaluate(uint8_t* str, int count);
void FORTH_type(uint8_t* str, int count);
void FORTH_cr(void);
int  FORTH_token(uint8_t **str);

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
 *      str   filename (w/ or w/o null termination)
 *  @param[in]
 *      count string length
 *  @return
 *      None
 */
void FS_include(uint8_t *str, int count) {
	FIL fil;        /* File object */
	char line[200]; /* Line buffer */
	FRESULT fr;     /* FatFs return code */

	memcpy(line, str, count);
	line[count] = 0;

	/* Open a text file */
	fr = f_open(&fil, line, FA_READ);
	if (fr) {
		// open failed
		Error_Handler();
	}

	/* Read every line and interprets it */
	while (f_gets(line, sizeof line, &fil)) {
		// line without \n
		FORTH_evaluate((uint8_t*)line, strlen(line)-1);
	}

	/* Close the file */
	f_close(&fil);
}


/**
 *  @brief
 *      Concatenate files and print on the standard output
 *  @param[in]
 *      str   filename (w/ or w/o null termination)
 *  @param[in]
 *      count string length
 *  @return
 *      None
 */
void FS_cat(uint8_t *str, int count) {
	FIL fil;        /* File object */
	char line[200]; /* Line buffer */
	FRESULT fr;     /* FatFs return code */

	memcpy(line, str, count);
	line[count] = 0;

	/* Open a text file */
	fr = f_open(&fil, line, FA_READ);
	if (fr) {
		// open failed
		Error_Handler();
	}

	FORTH_cr();
	/* Read every line and type it */
	while (f_gets(line, sizeof line, &fil)) {
		FORTH_type((uint8_t*)line, strlen(line));
	}

	/* Close the file */
	f_close(&fil);
}


///**
// *  @brief
// *      List directory contents.
// *  @param[in]
// *      str   directory (w/ or w/o null termination)
// *  @param[in]
// *      count string length
// *  @return
// *      None
// */
//void FS_ls(uint8_t *str, int count) {
//	char line[200]; /* Line buffer */
//	char pattern[20];
//	FILINFO fno;    /* File information */
//	FRESULT fr;     /* FatFs return code */
//	DIR dj;         /* Directory object */
//
//	memcpy(line, str, count);
//	line[count] = 0;
//
//	if (strchr(line, '*') != NULL || strchr(line, '?') != NULL) {
//		// there is a matching pattern string
//		if (strrchr(line, '/') == NULL) {
//			// no path, only pattern
//			strncpy(pattern, line, sizeof(pattern));
//			strcpy(line, "");
//		} else {
//			// path and pattern
//			strncpy(pattern, strrchr(line, '/')+1, sizeof(pattern));
//			line[strlen(line) - strlen(pattern)] = 0;
//		}
//	} else {
//		strcpy(pattern, "*");
//	}
//
//	fr = f_findfirst(&dj, &fno, line, pattern);
//
//	FORTH_cr();
//	while (fr == FR_OK && fno.fname[0]) {
//		/* Repeat while an item is found */
//		FORTH_type((uint8_t*)fno.fname, strlen(fno.fname));
//		FORTH_cr();
//		/* Search for next item */
//		fr = f_findnext(&dj, &fno);
//	}
//
//	f_closedir(&dj);
//}

void FS_ls(void) {
	char attrib[6];
	uint8_t *str = NULL;
	int count = 1;
	uint8_t a_flag = FALSE;
	uint8_t l_flag = FALSE;
	uint8_t param = FALSE;
	FRESULT fr;     /* FatFs return code */

	while (TRUE) {
		// get tokens till end of line
		count = FORTH_token(&str);
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

	FORTH_cr();
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
			if ( (fno.fattrib & AM_RDO) != AM_RDO) {
				attrib[2] = 'w';
			}
			if ( (fno.fattrib & AM_ARC) == AM_ARC) {
				attrib[3] = 'a';
			}

			snprintf(line, sizeof(line), "%s %9u %s\n",
					attrib, (unsigned int)fno.fsize, fno.fname);
		} else {
			snprintf(line, sizeof(line), "%s\n", fno.fname);
		}
		if ( ( (fno.fattrib & AM_HID) != AM_HID) || a_flag) {
			FORTH_type((uint8_t*)line, strlen(line));
		}
		/* Search for next item */
		fr = f_findnext(&dj, &fno);
	}

	f_closedir(&dj);
}

/**
 *  @brief
 *      Change the working directory.
 *  @param[in]
 *      str   directory (w/ or w/o null termination)
 *  @param[in]
 *      count string length
 *  @return
 *      None
 */
void FS_cd(uint8_t *str, int count) {
	char line[200]; /* Line buffer */
	FRESULT fr;     /* FatFs return code */

	memcpy(line, str, count);
	line[count] = 0;

	fr = f_chdir(line);
	if (fr != FR_OK) {
		strcpy(line, "Err: directory not found");
		FORTH_type((uint8_t*)line, strlen(line));
	}

}
/**
 *  @brief
 *      Print working directory
 *  @return
 *      None
 */
void FS_pwd(void) {
	TCHAR line[200];
	FRESULT fr;     /* FatFs return code */

	FORTH_cr();
	fr = f_getcwd(line, 200);  /* Get current directory path */
	if (fr == FR_OK) {
		FORTH_type((uint8_t*)line, strlen(line));
	} else {
		strcpy(line, "Err: no working directory");
		FORTH_type((uint8_t*)line, strlen(line));
	}
}


// Private Functions
// *****************


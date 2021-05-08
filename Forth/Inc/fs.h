/*
 * fs.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_FS_H_
#define INC_FS_H_

#include "ff.h"

extern const char FS_Version[];
extern	uint32_t **ZweitDictionaryPointer;

extern int EvaluateState;


void FS_init(void);

int FS_FIL_size(void);
int FS_FATFS_size(void);
int FS_DIR_size(void);
int FS_FILINFO_size(void);
int FS_FILINFO_fsize(void);
int FS_FILINFO_fdate(void);
int FS_FILINFO_ftime(void);
int FS_FILINFO_fattrib(void);
int FS_FILINFO_fname(void);
int FS_FILINFO_altname(void);

int FS_f_eof(FIL* fp);
FSIZE_t FS_f_size(FIL* fp);
int FS_f_error(FIL* fp);
int FS_getc(FIL* fp);

uint64_t FS_include  (uint64_t forth_stack, uint8_t *str, int count);
uint64_t FS_cat      (uint64_t forth_stack);
uint64_t FS_ls       (uint64_t forth_stack);
uint64_t FS_cd       (uint64_t forth_stack);
uint64_t FS_pwd      (uint64_t forth_stack);
uint64_t FS_mkdir    (uint64_t forth_stack);
uint64_t FS_rm       (uint64_t forth_stack);
uint64_t FS_mv       (uint64_t forth_stack);
uint64_t FS_cp       (uint64_t forth_stack);
uint64_t FS_split    (uint64_t forth_stack);
uint64_t FS_wc       (uint64_t forth_stack);
uint64_t FS_chmod    (uint64_t forth_stack);
uint64_t FS_touch    (uint64_t forth_stack);
uint64_t FS_mount    (uint64_t forth_stack);
uint64_t FS_umount   (uint64_t forth_stack);
uint64_t FS_chdrv    (uint64_t forth_stack);
uint64_t FS_mkfs     (uint64_t forth_stack);
uint64_t FS_dd		 (uint64_t forth_stack);

uint64_t FS_evaluate (uint64_t forth_stack, uint8_t *str, int count);
uint64_t FS_catch_evaluate (uint64_t forth_stack, uint8_t *str, int count);
uint64_t FS_type     (uint64_t forth_stack, uint8_t *str, int count);
uint64_t FS_cr       (uint64_t forth_stack);
uint64_t FS_token    (uint64_t forth_stack, uint8_t **str, int *count);
uint64_t FS_accept   (uint64_t forth_stack, uint8_t *str, int *count);


#endif /* INC_FS_H_ */

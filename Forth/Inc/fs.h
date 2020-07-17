/*
 * fs.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_FS_H_
#define INC_FS_H_


void FS_init(void);
void FS_include  (uint32_t psp, uint32_t tos, uint8_t *str, int count);
void FS_cat      (uint32_t psp, uint32_t tos);
void FS_ls       (uint32_t psp, uint32_t tos);
void FS_cd       (uint32_t psp, uint32_t tos);
void FS_pwd      (uint32_t psp, uint32_t tos);

void FS_evaluate (uint32_t psp, uint32_t tos, uint8_t* str, int count);
void FS_type     (uint32_t psp, uint32_t tos, uint8_t* str, int count);
void FS_cr       (uint32_t psp, uint32_t tos);
int  FS_token    (uint32_t psp, uint32_t tos, uint8_t **str);

#endif /* INC_FS_H_ */

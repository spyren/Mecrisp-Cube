/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * @file    user_diskio.c
  * @brief   This file includes a diskio driver skeleton to be completed by the user.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
 /* USER CODE END Header */

#ifdef USE_OBSOLETE_USER_CODE_SECTION_0
/*
 * Warning: the user section 0 is no more in use (starting from CubeMx version 4.16.0)
 * To be suppressed in the future.
 * Kept to ensure backward compatibility with previous CubeMx versions when
 * migrating projects.
 * User code previously added there should be copied in the new user sections before
 * the section contents can be deleted.
 */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */
#endif

/* USER CODE BEGIN DECL */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"

#include "sd.h"
#include "fd.h"
#include "bsp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* USER CODE END DECL */

/* Private function prototypes -----------------------------------------------*/
DSTATUS USER_SD_initialize (BYTE pdrv);
DSTATUS USER_SD_status (BYTE pdrv);
DRESULT USER_SD_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
  DRESULT USER_SD_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT USER_SD_ioctl (BYTE pdrv, BYTE cmd, void *buff);
#endif /* _USE_IOCTL == 1 */

DSTATUS USER_FD_initialize (BYTE pdrv);
DSTATUS USER_FD_status (BYTE pdrv);
DRESULT USER_FD_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
  DRESULT USER_FD_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT USER_FD_ioctl (BYTE pdrv, BYTE cmd, void *buff);
#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef  USER_SD_Driver =
{
  USER_SD_initialize,
  USER_SD_status,
  USER_SD_read,
#if  _USE_WRITE
  USER_SD_write,
#endif  /* _USE_WRITE == 1 */
#if  _USE_IOCTL == 1
  USER_SD_ioctl,
#endif /* _USE_IOCTL == 1 */
};

Diskio_drvTypeDef  USER_FD_Driver =
{
  USER_FD_initialize,
  USER_FD_status,
  USER_FD_read,
#if  _USE_WRITE
  USER_FD_write,
#endif  /* _USE_WRITE == 1 */
#if  _USE_IOCTL == 1
  USER_FD_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a SD Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_SD_initialize (
	BYTE pdrv           /* Physical drive nmuber to identify the drive */
)
{
  /* USER CODE BEGIN INIT */
	Stat = STA_NOINIT;
//	SD_init();
	if (SD_getBlocks() == 0) {
		// no SD card
		Stat = STA_NODISK;
	} else {
		Stat = 0;
	}
	return Stat;
  /* USER CODE END INIT */
}

/**
  * @brief  Gets SD Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_SD_status (
	BYTE pdrv       /* Physical drive number to identify the drive */
)
{
  /* USER CODE BEGIN STATUS */
	Stat = STA_NOINIT;
	// SD drive
	if (SD_getBlocks() == 0) {
		// no SD card
		Stat = STA_NODISK;
	} else {
		Stat = 0;
	}
	return Stat;
  /* USER CODE END STATUS */
}

/**
  * @brief  SD Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USER_SD_read (
	BYTE pdrv,      /* Physical drive nmuber to identify the drive */
	BYTE *buff,     /* Data buffer to store read data */
	DWORD sector,   /* Sector address in LBA */
	UINT count      /* Number of sectors to read */
)
{
  /* USER CODE BEGIN READ */
	DRESULT res = RES_ERROR;
	// SD drive
	if( SD_ReadBlocks((uint8_t*)buff, (uint32_t) (sector), count) == SD_OK) {
		res = RES_OK;
	}
	return res;
  /* USER CODE END READ */
}

/**
  * @brief  SD Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USER_SD_write (
	BYTE pdrv,          /* Physical drive nmuber to identify the drive */
	const BYTE *buff,   /* Data to be written */
	DWORD sector,       /* Sector address in LBA */
	UINT count          /* Number of sectors to write */
)
{
  /* USER CODE BEGIN WRITE */
	/* USER CODE HERE */
	DRESULT res = RES_ERROR;
	// SD drive
	if (SD_WriteBlocks((uint8_t*)buff, (uint32_t) (sector), count) == SD_OK) {
		res = RES_OK;
	}
	return res;
  /* USER CODE END WRITE */
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  SD I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USER_SD_ioctl (
	BYTE pdrv,      /* Physical drive nmuber (0..) */
	BYTE cmd,       /* Control code */
	void *buff      /* Buffer to send/receive control data */
)
{
  /* USER CODE BEGIN IOCTL */
	DRESULT res = RES_ERROR;
	SD_CardInfo CardInfo;
	// SD drive
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	switch (cmd) {
	/* Make sure that no pending write process */
	case CTRL_SYNC :
		res = RES_OK;
		break;

		/* Get number of sectors on the disk (DWORD) */
	case GET_SECTOR_COUNT :
		SD_GetCardInfo(&CardInfo);
		*(DWORD*)buff = CardInfo.LogBlockNbr;
		res = RES_OK;
		break;

		/* Get R/W sector size (WORD) */
	case GET_SECTOR_SIZE :
		SD_GetCardInfo(&CardInfo);
		*(WORD*)buff = CardInfo.LogBlockSize;
		res = RES_OK;
		break;

		/* Get erase block size in unit of sector (DWORD) */
	case GET_BLOCK_SIZE :
		SD_GetCardInfo(&CardInfo);
		*(DWORD*)buff = CardInfo.LogBlockSize / SD_BLOCK_SIZE;
		res = RES_OK;
		break;

	default:
		res = RES_PARERR;
	}

	return res;
  /* USER CODE END IOCTL */
}
#endif /* _USE_IOCTL == 1 */


/**
  * @brief  Initializes a Flash Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_FD_initialize (
	BYTE pdrv           /* Physical drive nmuber to identify the drive */
)
{
  /* USER CODE BEGIN INIT */
	Stat = STA_NOINIT;
	// flash drive
//	FD_init();
	if (FD_getBlocks() == 0) {
		// no flash
		Stat = STA_NODISK;
	} else {
		Stat = 0;
	}
	return Stat;
  /* USER CODE END INIT */
}

/**
  * @brief  Gets Flash Drive Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_FD_status (
	BYTE pdrv       /* Physical drive number to identify the drive */
)
{
  /* USER CODE BEGIN STATUS */
	Stat = STA_NOINIT;
	if (FD_getBlocks() == 0) {
		// no flash
		Stat = STA_NODISK;
	} else {
		Stat = 0;
	}
	return Stat;
  /* USER CODE END STATUS */
}

/**
  * @brief  Reads Flash Drive Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USER_FD_read (
	BYTE pdrv,      /* Physical drive nmuber to identify the drive */
	BYTE *buff,     /* Data buffer to store read data */
	DWORD sector,   /* Sector address in LBA */
	UINT count      /* Number of sectors to read */
)
{
  /* USER CODE BEGIN READ */
	DRESULT res = RES_ERROR;
	// flash drive
	if( FD_ReadBlocks((uint8_t*)buff, (uint32_t) (sector), count) == SD_OK) {
		res = RES_OK;
	}
	return res;
  /* USER CODE END READ */
}

/**
  * @brief  Writes Flash Drive Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USER_FD_write (
	BYTE pdrv,          /* Physical drive number to identify the drive */
	const BYTE *buff,   /* Data to be written */
	DWORD sector,       /* Sector address in LBA */
	UINT count          /* Number of sectors to write */
)
{
  /* USER CODE BEGIN WRITE */
	/* USER CODE HERE */
	DRESULT res = RES_ERROR;
	if (FD_WriteBlocks((uint8_t*)buff, (uint32_t) (sector), count) == SD_OK) {
		res = RES_OK;
	}
	return res;
  /* USER CODE END WRITE */
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  Flash Drive I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USER_FD_ioctl (
	BYTE pdrv,      /* Physical drive nmuber (0..) */
	BYTE cmd,       /* Control code */
	void *buff      /* Buffer to send/receive control data */
)
{
  /* USER CODE BEGIN IOCTL */
	DRESULT res = RES_ERROR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	switch (cmd) {
	/* Make sure that no pending write process */
	case CTRL_SYNC :
		res = RES_OK;
		break;

		/* Get number of sectors on the disk (DWORD) */
	case GET_SECTOR_COUNT :
		*(DWORD*)buff = (FD_END_ADDRESS - FD_START_ADDRESS) / FD_BLOCK_SIZE;
		res = RES_OK;
		break;

		/* Get R/W sector size (WORD) */
	case GET_SECTOR_SIZE :
		*(WORD*)buff = FD_BLOCK_SIZE;
		res = RES_OK;
		break;

		/* Get erase block size in unit of sector (DWORD) */
	case GET_BLOCK_SIZE :
		*(DWORD*)buff = 1;
		res = RES_OK;
		break;

	default:
		res = RES_PARERR;
	}


	return res;
  /* USER CODE END IOCTL */
}
#endif /* _USE_IOCTL == 1 */


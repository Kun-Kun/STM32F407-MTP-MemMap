/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * @file    user_diskio.c
  * @brief   This file includes a diskio driver skeleton to be completed by the user.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "user_diskio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;
extern uint8_t msc_ram[STORAGE_BLK_NBR][STORAGE_BLK_SIZ];

/* USER CODE END DECL */

/* Private function prototypes -----------------------------------------------*/
DSTATUS USER_initialize (BYTE pdrv);
DSTATUS USER_status (BYTE pdrv);
DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
  DRESULT USER_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff);
#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef  USER_Driver =
{
  USER_initialize,
  USER_status,
  USER_read,
#if  _USE_WRITE
  USER_write,
#endif  /* _USE_WRITE == 1 */
#if  _USE_IOCTL == 1
  USER_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_initialize (
	BYTE pdrv           /* Physical drive nmuber to identify the drive */
)
{
  /* USER CODE BEGIN INIT */
    Stat = 0;
    return Stat;
  /* USER CODE END INIT */
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_status (
	BYTE pdrv       /* Physical drive number to identify the drive */
)
{
  /* USER CODE BEGIN STATUS */
    Stat = 0;
    return Stat;
  /* USER CODE END STATUS */
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USER_read (
	BYTE pdrv,      /* Physical drive nmuber to identify the drive */
	BYTE *buff,     /* Data buffer to store read data */
	DWORD sector,   /* Sector address in LBA */
	UINT count      /* Number of sectors to read */
)
{
  /* USER CODE BEGIN READ */
	//memcpy(buff, msc_ram[sector], count * STORAGE_BLK_SIZ);

    return RES_OK;
  /* USER CODE END READ */
}

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USER_write (
	BYTE pdrv,          /* Physical drive nmuber to identify the drive */
	const BYTE *buff,   /* Data to be written */
	DWORD sector,       /* Sector address in LBA */
	UINT count          /* Number of sectors to write */
)
{
  /* USER CODE BEGIN WRITE */
	//memcpy(msc_ram[sector], buff, count * STORAGE_BLK_SIZ);

  /* USER CODE HERE */
    return RES_OK;
  /* USER CODE END WRITE */
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USER_ioctl (
	BYTE pdrv,      /* Physical drive nmuber (0..) */
	BYTE cmd,       /* Control code */
	void *buff      /* Buffer to send/receive control data */
)
{
  /* USER CODE BEGIN IOCTL */
 DRESULT res;
 BYTE n, csd[16], *ptr = buff;
 WORD csize;

 if (pdrv)
  return RES_PARERR;

 res = RES_ERROR;

 if (cmd == CTRL_POWER)
 {
  switch (*ptr) {
  case 0: /* Sub control code == 0 (POWER_OFF) */
  // if (chk_power())
   // power_off(); /* Power off */
   res = RES_OK;
   break;
  case 1: /* Sub control code == 1 (POWER_ON) */
   //power_on(); /* Power on */
   res = RES_OK;
   break;
  case 2: /* Sub control code == 2 (POWER_GET) */
   //*(ptr + 1) = (BYTE) chk_power();
   res = RES_OK;
   break;
  default:
   res = RES_PARERR;
  }
 }
 else
 {
  if (Stat & STA_NOINIT)
   return RES_NOTRDY;

  //SELECT(); /* CS = L */

  switch (cmd) {
  case GET_SECTOR_COUNT: /* Get number of sectors on the disk (DWORD) */
   *(DWORD*) buff = STORAGE_BLK_NBR;
   res = RES_OK;
   break;

  case GET_SECTOR_SIZE: /* Get sectors on the disk (WORD) */
   *(WORD*) buff = STORAGE_BLK_SIZ;
   res = RES_OK;
   break;

  case GET_BLOCK_SIZE: /* Get sectors on the disk (WORD) */
   *(WORD*) buff = 1;
   res = RES_OK;
   break;

  case CTRL_SYNC: /* Make sure that data has been written */
   //if (wait_ready() == 0xFF)
    res = RES_OK;
   break;

  case MMC_GET_CSD: /* Receive CSD as a data block (16 bytes) */
   //if (send_cmd(CMD9, 0) == 0 /* READ_CSD */
   //&& rcvr_datablock(ptr, 16))
    res = RES_OK;
   break;

  case MMC_GET_CID: /* Receive CID as a data block (16 bytes) */
   //if (send_cmd(CMD10, 0) == 0 /* READ_CID */
   //&& rcvr_datablock(ptr, 16))
    res = RES_OK;
   break;

  case MMC_GET_OCR: /* Receive OCR as an R3 resp (4 bytes) */
   //if (send_cmd(CMD58, 0) == 0)
   //{ /* READ_OCR */
    //for (n = 0; n < 4; n++)
   //  *ptr++ = rcvr_spi();
    res = RES_OK;
   //}

  default:
   res = RES_PARERR;
  }

  //DESELECT(); /* CS = H */
 // rcvr_spi(); /* Idle (Release DO) */
 }

 return res;
  /* USER CODE END IOCTL */
}
#endif /* _USE_IOCTL == 1 */


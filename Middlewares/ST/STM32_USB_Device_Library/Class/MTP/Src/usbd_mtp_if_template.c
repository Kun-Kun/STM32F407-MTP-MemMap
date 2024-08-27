/**
 ******************************************************************************
 * @file    usbd_mtp_if.c
 * @author  MCD Application Team
 * @brief   Source file for USBD MTP file list_files.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <memproxy_flash.h>
#include "usbd_mtp_if_template.h"

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*
 static FILE MyFile;
 static FATFS SDFatFs;
 static char SDPath[4];
 static FolderLevel Fold_Lvl;
 static FOLD_INFTypeDef FoldStruct;
 static FILE_INFTypeDef FileStruct;
 static SD_Object_TypeDef sd_object;
 */
extern USBD_HandleTypeDef USBD_Device;

uint32_t idx[200];
uint32_t parent;
/* static char path[255]; */
uint32_t sc_buff[MTP_IF_SCRATCH_BUFF_SZE / 4U];
uint32_t sc_len = 0U;
uint32_t pckt_cnt = 1U;
uint32_t foldsize;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static uint8_t USBD_MTP_Itf_Init(void);
static uint8_t USBD_MTP_Itf_DeInit(void);
static uint32_t USBD_MTP_Itf_ReadData(uint32_t Param1, uint8_t *buff,
		MTP_DataLengthTypeDef *data_length);
static uint16_t USBD_MTP_Itf_Create_NewObject(MTP_ObjectInfoTypeDef ObjectInfo,
		uint32_t objhandle);

static uint32_t USBD_MTP_Itf_GetIdx(uint32_t Param3, uint32_t *obj_handle);
static uint32_t USBD_MTP_Itf_GetParentObject(uint32_t Param);
static uint16_t USBD_MTP_Itf_GetObjectFormat(uint32_t Param);
static uint8_t USBD_MTP_Itf_GetObjectName_len(uint32_t Param);
static void USBD_MTP_Itf_GetObjectName(uint32_t Param, uint8_t obj_len,
		uint16_t *buf);
static uint32_t USBD_MTP_Itf_GetObjectSize(uint32_t Param);
static uint64_t USBD_MTP_Itf_GetMaxCapability(void);
static uint64_t USBD_MTP_Itf_GetFreeSpaceInBytes(void);
static uint32_t USBD_MTP_Itf_GetNewIndex(uint16_t objformat);
static void USBD_MTP_Itf_WriteData(uint16_t len, uint8_t *buff);
static uint32_t USBD_MTP_Itf_GetContainerLength(uint32_t Param1);
static uint16_t USBD_MTP_Itf_DeleteObject(uint32_t Param1);

static void USBD_MTP_Itf_Cancel(uint32_t Phase);
/* static uint32_t USBD_MTP_Get_idx_to_delete(uint32_t Param, uint8_t *tab); */

USBD_MTP_ItfTypeDef USBD_MTP_fops = { USBD_MTP_Itf_Init, USBD_MTP_Itf_DeInit,
		USBD_MTP_Itf_ReadData, USBD_MTP_Itf_Create_NewObject,
		USBD_MTP_Itf_GetIdx, USBD_MTP_Itf_GetParentObject,
		USBD_MTP_Itf_GetObjectFormat, USBD_MTP_Itf_GetObjectName_len,
		USBD_MTP_Itf_GetObjectName, USBD_MTP_Itf_GetObjectSize,
		USBD_MTP_Itf_GetMaxCapability, USBD_MTP_Itf_GetFreeSpaceInBytes,
		USBD_MTP_Itf_GetNewIndex, USBD_MTP_Itf_WriteData,
		USBD_MTP_Itf_GetContainerLength, USBD_MTP_Itf_DeleteObject,
		USBD_MTP_Itf_Cancel, sc_buff,
		MTP_IF_SCRATCH_BUFF_SZE, };

/* Private functions ---------------------------------------------------------*/

struct array_16_s {
	uint16_t size;
	uint16_t *buff;
};

static struct array_16_s convertChars(char *str) {
	int length = strlen(str) + 1;
	uint16_t buff[length];
	uint8_t *ptr = (uint8_t*) &buff; //cast the 16bit pointer to an 8bit pointer
	for (int i = 0; i < length; i++) {
		*ptr = (char) str[i];
		ptr++;          //move your pointer
		*ptr = (char) 0;
		ptr++;          //move your pointer
	}
	struct array_16_s ret = { .size = length, .buff = buff };
	return ret;
}

/**
 * @brief  USBD_MTP_Itf_Init
 *         Initialize the file system Layer
 * @param  None
 * @retval status value
 */

static uint8_t USBD_MTP_Itf_Init(void) {
	return init_hardware();
}

/**
 * @brief  USBD_MTP_Itf_DeInit
 *         Uninitialize the file system Layer
 * @param  None
 * @retval status value
 */
static uint8_t USBD_MTP_Itf_DeInit(void) {
	flush();
	return 0;
}

/**
 * @brief  USBD_MTP_Itf_GetIdx
 *         Get all object handle
 * @param  Param3: current object handle
 * @param  obj_handle: all objects handle (subfolders/files) in current object
 * @retval number of object handle in current object
 */
static uint32_t USBD_MTP_Itf_GetIdx(uint32_t Param3, uint32_t *obj_handle) {
	uint32_t count = 1U;
	obj_handle[0] = 1;
	return count;
}

/**
 * @brief  USBD_MTP_Itf_GetParentObject
 *         Get parent object
 * @param  Param: object handle (object index)
 * @retval parent object
 */
static uint32_t USBD_MTP_Itf_GetParentObject(uint32_t Param) {
	uint32_t parentobj = 0U;
	return parentobj;
}

/**
 * @brief  USBD_MTP_Itf_GetObjectFormat
 *         Get object format
 * @param  Param: object handle (object index)
 * @retval object format
 */
static uint16_t USBD_MTP_Itf_GetObjectFormat(uint32_t Param) {
	uint16_t objformat = 0U;
	return objformat;
}

/**
 * @brief  USBD_MTP_Itf_GetObjectName_len
 *         Get object name length
 * @param  Param: object handle (object index)
 * @retval object name length
 */
static uint8_t USBD_MTP_Itf_GetObjectName_len(uint32_t Param) {
	if (Param == 1) {
		struct array_16_s filename = convertChars("flash.bin");
		return (filename.size);
	}
	return 0;
}

/**
 * @brief  USBD_MTP_Itf_GetObjectName
 *         Get object name
 * @param  Param: object handle (object index)
 * @param  obj_len: length of object name
 * @param  buf: pointer to object name
 * @retval object size in SD card
 */
static void USBD_MTP_Itf_GetObjectName(uint32_t Param, uint8_t obj_len,
		uint16_t *buf) {
	if (Param == 1) {
		struct array_16_s filename = convertChars("flash.bin");
		memcpy(buf, filename.buff, filename.size * (sizeof filename.buff[0]));
		return;
	}
	return;
}

/**
 * @brief  USBD_MTP_Itf_GetObjectSize
 *         Get size of current object
 * @param  Param: object handle (object index)
 * @retval object size in SD card
 */
static uint32_t USBD_MTP_Itf_GetObjectSize(uint32_t Param) {
	if (Param == 1) {
		return getMemSize();
	}
	return 0;
}

/**
 * @brief  USBD_MTP_Itf_Create_NewObject
 *         Create new object in SD card and store necessary information for future use
 * @param  ObjectInfo: object information to use
 * @param  objhandle: object handle (object index)
 * @retval None
 */
static uint16_t USBD_MTP_Itf_Create_NewObject(MTP_ObjectInfoTypeDef ObjectInfo,
		uint32_t objhandle) {
	uint16_t rep_code = MTP_RESPONSE_OK;
	init_write(ObjectInfo.ObjectCompressedSize);
	return rep_code;
}

/**
 * @brief  USBD_MTP_Itf_GetMaxCapability
 *         Get max capability in SD card
 * @param  None
 * @retval max capability
 */
static uint64_t USBD_MTP_Itf_GetMaxCapability(void) {
	uint64_t max_cap = 1024 * 1024U;

	return max_cap;
}

/**
 * @brief  USBD_MTP_Itf_GetFreeSpaceInBytes
 *         Get free space in bytes in SD card
 * @param  None
 * @retval free space in bytes
 */
static uint64_t USBD_MTP_Itf_GetFreeSpaceInBytes(void) {
	uint64_t f_space_inbytes = 512 * 1024U;

	return f_space_inbytes;
}

/**
 * @brief  USBD_MTP_Itf_GetNewIndex
 *         Create new object handle
 * @param  objformat: object format
 * @retval object handle
 */
static uint32_t USBD_MTP_Itf_GetNewIndex(uint16_t objformat) {
	return 2;
}

/**
 * @brief  USBD_MTP_Itf_WriteData
 *         Write file data to SD card
 * @param  len: size of data to write
 * @param  buff: data to write in SD card
 * @retval None
 */
static void USBD_MTP_Itf_WriteData(uint16_t len, uint8_t *buff) {
	if(len==0){
		flush();
		return;
	}
	write_to_mem(buff, len);
	return;
}

/**
 * @brief  USBD_MTP_Itf_GetContainerLength
 *         Get length of generic container
 * @param  Param1: object handle
 * @retval length of generic container
 */
static uint32_t USBD_MTP_Itf_GetContainerLength(uint32_t Param1) {
	uint32_t length = 0U;
	UNUSED(Param1);

	return length;
}

/**
 * @brief  USBD_MTP_Itf_DeleteObject
 *         delete object from SD card
 * @param  Param1: object handle (file/folder index)
 * @retval response code
 */
static uint16_t USBD_MTP_Itf_DeleteObject(uint32_t Param1) {
	uint16_t rep_code = MTP_RESPONSE_OK;
	clean_mem();
	return rep_code;
}

/**
 * @brief  USBD_MTP_Get_idx_to_delete
 *         Get all files/foldres index to delete with descending order ( max depth)
 * @param  Param: object handle (file/folder index)
 * @param  tab: pointer to list of files/folders to delete
 * @retval Number of files/folders to delete
 */
/* static uint32_t USBD_MTP_Get_idx_to_delete(uint32_t Param, uint8_t *tab)
 {
 uint32_t cnt = 0U;

 return cnt;
 }
 */

/**
 * @brief  USBD_MTP_Itf_ReadData
 *         Read data from SD card
 * @param  Param1: object handle
 * @param  buff: pointer to data to be read
 * @param  temp_length: current data size read
 * @retval necessary information for next read/finish reading
 */
static uint32_t USBD_MTP_Itf_ReadData(uint32_t Param1, uint8_t *buff,
		MTP_DataLengthTypeDef *data_length) {
	if (Param1 == 1) {
		uint32_t readBlock;
		if(data_length->temp_length == 0){
			init_read(getMemSize());
			readBlock=52;
		}else{
			readBlock = 64;
		}
		uint32_t toRead;
		data_length->totallen = getMemSize();
		if (data_length->totallen - data_length->temp_length <= readBlock) {
			toRead = data_length->totallen - data_length->temp_length;
		} else {
			toRead = readBlock;
		}
		uint32_t already = data_length->temp_length;
		read_from_mem(buff, toRead);
		data_length->readbytes = toRead;
		data_length->temp_length = already + toRead;
	}


	return 0U;
}

/**
 * @brief  USBD_MTP_Itf_Cancel
 *         Close opened folder/file while cancelling transaction
 * @param  MTP_ResponsePhase: MTP current state
 * @retval None
 */
static void USBD_MTP_Itf_Cancel(uint32_t Phase) {
	UNUSED(Phase);

	/* Make sure to close open file while canceling transaction */

	return;
}

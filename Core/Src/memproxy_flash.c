/*
 * memproxy.c
 *
 *  Created on: 24 серп. 2024 р.
 *      Author: 38063
 */

#include <memproxy_flash.h>
#include "w25qxx.h"

#define RTX_BUFFER_SIZE 1024

uint16_t buff_used = 0;
uint8_t rtx_buffer[RTX_BUFFER_SIZE];
int16_t read_buff_pointer = 0;

uint32_t mem_pointer = 0;
uint8_t is_write = 0;

uint16_t mem_page_size = 256;
uint32_t mem_size = 0;

uint8_t ableToWrite = 0;



extern w25qxx_t w25qxx;
extern I2C_HandleTypeDef hi2c2;

void write_mem_internal(uint32_t mem_pos, uint8_t *buff, uint16_t size) {
	uint32_t page_addr = mem_pos / mem_page_size;
	if (W25qxx_IsEmptyPage(page_addr, 0, w25qxx.PageSize)) {
		W25qxx_WritePage(buff, mem_pos / mem_page_size, 0, mem_page_size);
	} else {
		uint32_t sector_addr = W25qxx_PageToSector(page_addr);
		W25qxx_EraseSector(sector_addr);
		W25qxx_WritePage(buff, mem_pos / mem_page_size, 0, mem_page_size);
	}
}
void read_mem_internal(uint32_t mem_pos, uint8_t *buff, uint16_t size) {
	W25qxx_ReadPage(buff, mem_pos / mem_page_size, 0, mem_page_size);
}

uint32_t getMemSize() {
	return mem_size;
}

uint32_t init_write(uint32_t size) {
	is_write = 1;
	flush();
	return size > mem_size ? mem_size : size;
}

uint32_t init_read(uint32_t size) {
	is_write = 0;
	flush();
	return size > mem_size ? mem_size : size;
}

void write_to_mem(uint8_t *buff, uint16_t size) {
	if (!is_write) {
		return;
	}
	memcpy(rtx_buffer + buff_used, buff, size);
	buff_used += size;
	if (buff_used > mem_page_size) {
		uint8_t pages = buff_used / mem_page_size;
		for (int i = 0; i < pages; i++) {
			if (mem_pointer >= mem_size) {
				return;
			}
			write_mem_internal(mem_pointer, rtx_buffer + (i * mem_page_size),
					mem_page_size);
		}
		uint16_t wrote_size = pages * mem_page_size;
		uint16_t data_rest = buff_used - wrote_size;
		if (data_rest) {
			memcpy(rtx_buffer, rtx_buffer + wrote_size, data_rest);
		}
		buff_used = data_rest;
		mem_pointer += wrote_size;
	}
}

void read_from_mem(uint8_t *buff, uint16_t size) {
	if (is_write) {
		return;
	}

	while (buff_used < size) {
		memcpy(rtx_buffer, rtx_buffer + read_buff_pointer, buff_used);
		read_buff_pointer = 0;
		if (mem_pointer >= mem_size) {
			return;
		}
		read_mem_internal(mem_pointer, rtx_buffer + buff_used, mem_page_size);
		buff_used += mem_page_size;
		mem_pointer += mem_page_size;
	}
	memcpy(buff, rtx_buffer + read_buff_pointer, size);
	buff_used -= size;
	read_buff_pointer += size;
}

void clean_mem() {
	W25qxx_EraseChip();
}

void flush() {
	if (is_write) {
		if (buff_used) {
			if (mem_pointer >= mem_size) {
				return;
			}
			write_mem_internal(mem_pointer, rtx_buffer, buff_used);
		}
	}
	buff_used = 0;
	read_buff_pointer = 0;
	mem_pointer = 0;
}

uint8_t init_hardware() {
	if (W25qxx_Init()) {
		mem_size = w25qxx.BlockCount * w25qxx.BlockSize;
		return 0;
	}
	return 1;
}


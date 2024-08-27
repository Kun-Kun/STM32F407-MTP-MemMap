/*
 * memproxy.c
 *
 *  Created on: 24 серп. 2024 р.
 *      Author: 38063
 */
/*
#include <memproxy_eeprom.h>
#include "ee24.h"

#define RTX_BUFFER_SIZE 1024

uint16_t buff_used = 0;
uint8_t rtx_buffer[RTX_BUFFER_SIZE];
int16_t read_buff_pointer = 0;

uint32_t mem_pointer = 0;
uint8_t is_write = 0;

uint16_t mem_page_size = 64;
uint32_t mem_size = 1024;

EE24_HandleTypeDef ee24;
extern I2C_HandleTypeDef hi2c2;

void write_mem_internal(uint32_t mem_pos, uint8_t *buff, uint16_t size){
	EE24_Write(&ee24, mem_pos, buff, size, 1000);
}
void read_mem_internal(uint32_t mem_pos, uint8_t *buff, uint16_t size){
    EE24_Read(&ee24, mem_pos, buff, size, 1000);
}

uint32_t getMemSize(){
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
			if(mem_pointer>=mem_size){
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
		if(mem_pointer>=mem_size){
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

void clean_mem(){
	uint8_t empty= 0xff;
	for (uint32_t i = 0; i < mem_size; i++) {
		EE24_Write(&ee24, i, &empty, 1, 1000);
	}
}


void flush() {
	if (is_write) {
		if (buff_used) {
			if(mem_pointer>=mem_size){
				return;
			}
			write_mem_internal(mem_pointer, rtx_buffer, buff_used);
		}
	}
	buff_used = 0;
	read_buff_pointer = 0;
	mem_pointer = 0;
}

uint8_t init_hardware(){
	if(EE24_Init(&ee24, &hi2c2, EE24_ADDRESS_DEFAULT)){
		return 0;
	}
	return 1;
}

*/


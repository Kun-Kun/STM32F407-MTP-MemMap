This project creates USB MTP device STP32F407. It has only one file that maps on memories such as eeprom 24c02 or flash 25q32.
It supports reading(opening/copy to host machine), writing to memory (Copying file into MTP device) erasing memory by deleting file.
It works in streaming mode and requires 1024 bytes of memory for buffer(may be reduced to depending on the page size)
It is the simple example how to work with MTP drivers for STM32 USB Device library.
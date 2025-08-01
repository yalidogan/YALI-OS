#ifndef CONFIG_H
#define CONFIG_H


#define KERNEL_CODE_SELECTOR 0X08
#define KERNEL_DATA_SELECTOR 0x10

#define YALIOS_TOTAL_INTERRUPTS 512

//100MB heap size
#define YALIOS_HEAP_SIZE_BYTES 104857600
#define YALIOS_HEAP_BLOCK_SIZE 4096
#define YALIOS_HEAP_ADDRESS 0x01000000
#define YALIOS_HEAP_TABLE_ADDRESS 0x00007E00

#define YALIOS_SECTOR_SIZE 512 


#endif
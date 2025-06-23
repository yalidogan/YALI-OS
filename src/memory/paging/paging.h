#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>



//Bitmasks for the page table entries 
#define PAGING_CACHE_DISABLED   0b00010000 //C bit in the page table entry -> cache disabled 
#define PAGING_WRITE_THROUGH    0b00001000
#define PAGING_ACCESS_FROM_ALL  0b00000100
#define PAGING_IS_WRITABLE      0b00000010
#define PAGING_IS_PRESENT       0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

struct paging_4gb_chunk
{
    uint32_t* directory_entry; 

};

void paging_switch(uint32_t* directory);
struct paging_4gb_chunk* paging_new_4gb(uint8_t flags);
void enable_paging(); //Enable paging with the asm code from osdev


int paging_set(uint32_t* directory, void* virt, uint32_t val);
bool paging_is_aligned(void* address);

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk);


#endif
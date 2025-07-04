#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "string/string.h"
#include "disk/streamer.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char colour)
{
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour)
{
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}

void terminal_writechar(char c, char colour)
{

    if(c == '\n')
    {
        terminal_row += 1; 
        terminal_col = 0; 
        return; 
    }
    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;

    if(terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row += 1; 
    }
}

//Goes through the terminal grid and clears the pixels for use 
void terminal_initialize()
{

    video_mem = (uint16_t*)(0xB8000);
    terminal_col = 0;
    terminal_row = 0; 
    for(int y = 0; y < VGA_HEIGHT; y++)
    {
        for(int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y,' ', 0);
        }
    }
}


void print(const char* str)
{
    size_t len = strlen(str);
    for(int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}

static struct paging_4gb_chunk* kernel_chunk = 0; 

void kernel_main()
{
    terminal_initialize();
    print("YALI OPERATING SYSTEM\n YERLI VE MILLI");

    //Init the heap 
    kheap_init();

    //Seach and initialize the disks 
    disk_search_and_init(); 

    //Init the interrupt descriptor table 
    idt_init();

    //Setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    //Switch to kernel paging chunk 
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    //Enable paging 
    enable_paging();

    //Enable the interrupts after we init the IDT and other operations -> so that system doesn't panic 
    enable_interrupts(); 

    struct disk_stream* stream = diskstreamer_new(0);
    diskstreamer_seek(stream, 0x201);
    unsigned char c = 0; 
    diskstreamer_read(stream, &c, 1);
    while(1)
    {
        
    }

} 
#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_desc
{
    uint16_t offset_1; //Offset bits 0-15
    uint16_t selector; //Selector thats in our gdt 
    uint8_t zero; //Nothing, unused set to zero 
    uint8_t type_attr; // Descr type and attributes 
    uint16_t offset_2; //Offset bits 16-31
} __attribute__((packed));


struct idtr_desc
{
    uint16_t limit; //Size of the desc table - 1
    uint32_t base; //Base addr of the start of the the interrupt descriptor table 

}__attribute__((packed));

void idt_init();


#endif
#include <stdint.h>

extern int main(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler(void);


#define STACK_SIZE 0x400
static uint32_t stack[STACK_SIZE];


void Reset_Handler(void) {
    main();
    while(1);
}


void Default_Handler(void) {
    while(1);
}


__attribute__((section(".isr_vector")))
const uint32_t vector_table[] = {
    (uint32_t)&stack[STACK_SIZE],  // Initial stack pointer
    (uint32_t)Reset_Handler,        // Reset
    (uint32_t)Default_Handler,      // NMI
    (uint32_t)Default_Handler,      // HardFault
    (uint32_t)Default_Handler,      // MemManage
    (uint32_t)Default_Handler,      // BusFault
    (uint32_t)Default_Handler,      // UsageFault
    0, 0, 0, 0,                     // Reserved
    (uint32_t)vPortSVCHandler,      // SVCall
    (uint32_t)Default_Handler,      // Debug Monitor
    0,                              // Reserved
    (uint32_t)xPortPendSVHandler,   // PendSV
    (uint32_t)xPortSysTickHandler,  // SysTick
};
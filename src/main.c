#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdint.h>
#include <stdio.h>

#define UART0_BASE 0x40013800 //BASE ADDRESS
#define UART_DR    (*((volatile uint32_t *)(UART0_BASE + 0x00))) //DATA REGISTER
#define UART_FR    (*((volatile uint32_t *)(UART0_BASE + 0x18))) //FLAG REGISTER


void uart_putc(char c) {
    while (UART_FR & (1 << 5));  //can accept more data or not, bit 5 of the FLAG register
    UART_DR = c;
}

void uart_puts(const char *str) {
    while (*str) { //WRITE CHARACTERS TO THE FLAG REGISTER 
        if (*str == '\n') uart_putc('\r');
        uart_putc(*str++);
    }
}


TaskHandle_t xTaskLED1Handle = NULL; //3 TASKS, LED1, LED2, MONITOR
TaskHandle_t xTaskLED2Handle = NULL;
TaskHandle_t xTaskMonitorHandle = NULL;

QueueHandle_t xMessageQueue; //THE QUEUE


SemaphoreHandle_t xMutex; //THE MUTEX


typedef struct {
    uint32_t taskID;
    uint32_t value;  //STRUCTURE FOR A MESSAGE(TASK,VALUE)
} Message_t;


void vTaskLED1(void *pvParameters) {
    Message_t msg;
    uint32_t counter = 0;

    uart_puts("[Task1] LED1 Task started\n");

    for (;;) {
    
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            uart_puts("[Task1] LED1 ON\n");
            xSemaphoreGive(xMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(500)); 

        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            uart_puts("[Task1] LED1 OFF\n");
            xSemaphoreGive(xMutex);
        }

        
        msg.taskID = 1;
        msg.value = counter++;
        xQueueSend(xMessageQueue, &msg, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


void vTaskLED2(void *pvParameters) {
    Message_t msg;
    uint32_t counter = 0;

    uart_puts("[Task2] LED2 Task started\n");

    for (;;) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            uart_puts("[Task2] LED2 TOGGLE\n");
            xSemaphoreGive(xMutex);
        }


        msg.taskID = 2;
        msg.value = counter++;
        xQueueSend(xMessageQueue, &msg, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

void vTaskMonitor(void *pvParameters) {
    Message_t rxMsg;
    char buffer[64];

    uart_puts("[Monitor] Monitor Task started\n");

    for (;;) {
        if (xQueueReceive(xMessageQueue, &rxMsg, portMAX_DELAY) == pdTRUE) {
            if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
                snprintf(buffer, sizeof(buffer),
                         "[Monitor] Received from Task%lu: value=%lu\n",
                         rxMsg.taskID, rxMsg.value);
                uart_puts(buffer);
                xSemaphoreGive(xMutex);
            }
        }
    }
}


int main(void) {
    uart_puts("\n=================================\n");
    uart_puts("FreeRTOS on ARM Cortex-M3 (QEMU)\n");
    uart_puts("=================================\n\n");

  
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        uart_puts("ERROR: Failed to create mutex\n");
        while(1);
    }

  
    xMessageQueue = xQueueCreate(10, sizeof(Message_t));
    if (xMessageQueue == NULL) {
        uart_puts("ERROR: Failed to create queue\n");
        while(1);
    }

    
    xTaskCreate(vTaskLED1, "LED1", 128, NULL, 2, &xTaskLED1Handle);
    xTaskCreate(vTaskLED2, "LED2", 128, NULL, 2, &xTaskLED2Handle);
    xTaskCreate(vTaskMonitor, "Monitor", 256, NULL, 3, &xTaskMonitorHandle);

    uart_puts("Tasks created, starting scheduler...\n\n");

    vTaskStartScheduler();

 
    uart_puts("ERROR: Scheduler failed to start\n");
    while(1);

    return 0;
}


void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    uart_puts("STACK OVERFLOW: ");
    uart_puts(pcTaskName);
    uart_puts("\n");
    while(1);
}

void vApplicationMallocFailedHook(void) {
    uart_puts("MALLOC FAILED\n");
    while(1);
}

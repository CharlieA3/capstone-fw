#ifndef LORA_THREAD
#define LORA_THREAD

#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>

#define LORA_STACK_SIZE 512
#define LORA_PRIO 4

extern struct kmsg_q lora_queue;

struct lora_readings
{
    int johnny;
    int marty;
};

// entry point for LoRa
void lora_thread(void *a1, void *a2, void *a3);

#endif
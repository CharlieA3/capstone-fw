#ifndef LORA_THREAD
#define LORA_THREAD

#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>

#define LORA_STACK_SIZE 512
#define LORA_PRIO 4

extern struct k_msgq sx1262_queue;

typedef struct __attribute__((packed))
{
    uint8_t identifier;
    int16_t temperature;
    uint16_t humidity;
    uint16_t pressure;
    uint32_t gas_resistance;
    uint16_t tvoc;
    uint16_t eco2;
    uint8_t aqi;
    uint8_t status;
} spi_sx1262_packet_t;

// entry point for LoRa
void lora_thread(void *a1, void *a2, void *a3);

bool check_heartbeat();

#endif
#ifndef LORA_THREAD_H
#define LORA_THREAD_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/lora.h>

#define LORA_STACK_SIZE 512
#define LORA_PRIO 4

extern struct k_msgq sx1262_queue;

typedef struct
{
    int32_t temperature;
    int32_t humidity;
    int32_t pressure;
    int32_t gas_resistance;
} bme688_data_packet_t;

typedef struct __attribute__((packed))
{
    uint8_t identifier;
    bme688_data_packet_t env_data;
    uint8_t fire_alert; // 0 (cold), 1 (hot), 2 (smoky/dangerous), 3-5 (fire)
} spi_sx1262_packet_t;

// entry point for LoRa
void lora_thread_entry_point(void *a1, void *a2, void *a3);
bool init_lora_node();
bool check_heartbeat();

#endif // LORA_THREAD_H
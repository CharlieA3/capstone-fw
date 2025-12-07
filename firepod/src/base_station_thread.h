#ifndef BASE_STATION_THREAD_H
#define BASE_STATION_THREAD_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/lora.h>

#define BASE_STATION_STACK_SIZE 512
#define BASE_STATION_PRIO 4

// Message queue for received packets
extern struct k_msgq sx1262_rx_queue;

// Received packet structure - matches transmit packet format
typedef struct __attribute__((packed))
{
    uint8_t identifier;
    int32_t temperature;
    int32_t humidity;
    int32_t pressure;
    int32_t gas_resistance;
} spi_sx1262_rx_packet_t;

// Entry point for LoRa receiver thread
void base_station_thread_entry_point(void *a1, void *a2, void *a3);

// Optional: Function to check if valid data has been received recently
bool check_rx_status(void);

#endif // BASE_STATION_THREAD_H
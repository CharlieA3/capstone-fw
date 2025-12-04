// #ifndef LORA_THREAD
// #define LORA_THREAD

// #include <zephyr/kernel.h>
// #include <zephyr/drivers/spi.h>

// #define LORA_STACK_SIZE 512
// #define LORA_PRIO 4

// extern struct k_msgq sx1262_queue;

// typedef struct __attribute__((packed))
// {
//     uint8_t identifier;
//     int16_t temperature;
//     uint16_t humidity;
//     uint16_t pressure;
//     uint32_t gas_resistance;
//     uint16_t tvoc;
//     uint16_t eco2;
//     uint8_t aqi;
//     uint8_t status;
// } spi_sx1262_packet_t;

// // entry point for LoRa
// void lora_thread(void *a1, void *a2, void *a3);

// bool check_heartbeat();

// #endif

// #ifndef LORA_THREAD_H
// #define LORA_THREAD_H

// #include <zephyr/kernel.h>
// #include <zephyr/drivers/lora.h>

// // Thread configuration
// #define LORA_STACK_SIZE 1024
// #define LORA_PRIO 5

// // Structure for received LoRa data
// struct lora_rx_data {
//     uint8_t data[256];      // Received payload
//     int16_t rssi;           // Signal strength
//     int8_t snr;             // Signal-to-noise ratio
//     uint16_t len;           // Payload length
// };

// // Message queue for received LoRa packets
// extern struct k_msgq lora_rx_queue;

// // Thread entry point
// void lora_thread(void *a1, void *a2, void *a3);

// // Utility function
// bool check_heartbeat(void);

// #endif // LORA_THREAD_H

#ifndef LORA_THREAD_H
#define LORA_THREAD_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/lora.h>

// LoRa thread configuration
#define LORA_STACK_SIZE 2048
#define LORA_PRIO 5

// LoRa packet structure
typedef struct {
    uint8_t data[255];      // Payload data
    uint8_t length;         // Payload length
    int8_t rssi;            // Received signal strength (for RX)
    int8_t snr;             // Signal-to-noise ratio (for RX)
    bool is_tx;             // true = transmit, false = receive
} spi_sx1262_packet_t;

// LoRa thread entry point
void lora_thread(void *msgq_ptr, void *unused1, void *unused2);

#endif // LORA_THREAD_H
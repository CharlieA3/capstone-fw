#ifndef SENSOR_THREAD_H
#define SENSOR_THREAD_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>

#define STACK_SIZE 2048
#define SENSOR_PRIO 3
#define CONSOLE_PRIO 6

struct bme688_readings
{
    int32_t temperature;       // Celsius
    int32_t humidity;          // %RH
    int32_t pressure;          // Pascals
    int32_t gas_resistance;    // Ohms
};

// Externally defined message queue used between sensor + console threads
extern struct k_msgq bme688_queue;

// Thread entry points
void sensor_reading_entry_point(void *a1, void *a2, void *a3);
void console_entry_point(void *a1, void *a2, void *a3);

#endif // SENSOR_THREAD_H

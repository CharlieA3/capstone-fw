#ifndef SENSOR_THREADS_H
#define SENSOR_THREADS_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>

#define STACK_SIZE 512
#define SENSOR_PRIO 5
#define CONSOLE_PRIO 5

struct bme688_readings
{
    uint32_t temp;
    uint16_t hum;
};

// externally defined message queue used between threads
extern struct k_msgq bme688_queue;

// thread entry points
void sensor_reading_entry_point(void *a1, void *a2, void *a3);
void console_entry_point(void *a1, void *a2, void *a3);

#endif

#include <zephyr/kernel.h>
#include "sensor_thread.h"
#include "lora_thread.h"

// global message queue
static char bme688_buffer[10 * sizeof(struct bme688_readings)];
struct k_msgq bme688_queue;

#define STACK_SIZE 2048

static char sx1262_msg_buffer[10 * sizeof(spi_sx1262_packet_t)];
struct k_msgq sx1262_queue;

K_THREAD_STACK_DEFINE(stack_area_1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_area_2, STACK_SIZE);
K_THREAD_STACK_DEFINE(lora_stack, LORA_STACK_SIZE);

struct k_thread i2c_reading_thread;
struct k_thread printing_thread;
struct k_thread lora_rf_thread;

// semaphore for triggering lora thread
struct k_sem lora_trigger_sem;

int main(void)
{
    // init message queue to pass to threads
    k_msgq_init(&bme688_queue, bme688_buffer, sizeof(struct bme688_readings), 10);
    k_msgq_init(&sx1262_queue, sx1262_msg_buffer, sizeof(spi_sx1262_packet_t), 10);

    k_sem_init(&lora_trigger_sem, 0, 1);

    // k_thread_create(&i2c_reading_thread,
    //                 stack_area_1,
    //                 K_THREAD_STACK_SIZEOF(stack_area_1),
    //                 sensor_reading_entry_point,
    //                 &bme688_queue, NULL, NULL,
    //                 SENSOR_PRIO, 0, K_NO_WAIT);

    k_thread_create(&printing_thread,
                    stack_area_2,
                    K_THREAD_STACK_SIZEOF(stack_area_2),
                    console_entry_point,
                    &bme688_queue, NULL, NULL,
                    CONSOLE_PRIO, 0, K_NO_WAIT);

    k_thread_create(&lora_rf_thread,
                    lora_stack,
                    LORA_STACK_SIZE,
                    lora_thread_entry_point,
                    &sx1262_msg_buffer, &lora_trigger_sem, NULL,
                    LORA_PRIO, 0, K_NO_WAIT);

    return 0;
}

// ------------------------ FirePod ---------------------------

// 1. Initialize message queues and threads

// 2. Spawn threads, immediately switch to sensor thread to check if they are running properly, populate a message queue

// 3. Once everything is chillin, go to sleepy mode

// Questions
// 1. Would sleep be it's own thread, or just a function I can call in the sensors thread? The goal here would be to wake up based on an interrupt

// ------------------------ Sensor thread ---------------------------

// 1. I2C initialization - does it make sense to have the sensor thread initialize the SCP drivers or should this be done before the thread is started?

// 2. SPI initialization

// 3. Read data from each sensor

// 4. check readings from each sensors to tell if they are within normal levels and add the average of the past x number of readings to their own message queue

// ------------------------ RF thread ---------------------------

// 1. Make sure RF chip is chillin

// 2. read data from the message queues for sensors or just fire rating (or we can actually calulate in this thread)

// 3. Structure data and use send function

// 4. Once sending is done, switch to receiving mode

// ------------------------ HW Malfunction thread ---------------------------
// #include <stdio.h>
#include <zephyr/kernel.h>
// zephyr/device.h is included eventually through kernel.h
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/lora.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(bme688, LOG_LEVEL_DBG);

#define STACK_SIZE 512
#define SENSOR_PRIO 4
#define CONSOLE_PRIO 5

#define BME DT_NODELABEL(bme688)

static const struct i2c_dt_spec bme688 = I2C_DT_SPEC_GET(BME);

// this is always underlined in red for some reason
K_THREAD_STACK_DEFINE(stack_area_1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_area_2, STACK_SIZE);

struct k_thread sensor_reading_thread;
struct k_thread printing_thread;

// do not need a semaphore for message queues because message queues are thread safe
// struct k_sem sensor_read_sem;

struct sensor_readings
{
    uint32_t temp;
    uint16_t hum;
};

char sensor_buffer[10 * sizeof(struct sensor_readings)];
struct k_msgq sensor_queue;

// could put extern in front of void why?
void sensor_reading_entry_point(void *, void *, void *)
{
    if (!device_is_ready(bme688.bus))
    {
        printk("Cannot interact with device: %s", bme688.bus->name);
    }

    struct sensor_readings readings;

    // confguring the bosch sensor with mode and oversampling rates for each sensor
    uint8_t ctrl_hum = 0x01;
    uint8_t ctrl_meas = (0x02 << 5) | (0x01 << 2) | 0x01;

    uint8_t temp_buf[3];
    uint32_t temp_reading;

    uint8_t hum_buf[2];
    uint16_t humidity_reading;

    int ret;

    while (true)
    {
        // since we are using forced mode, the sensor needs to be written to every time we want to read a new value
        i2c_reg_write_byte_dt(&bme688, 0x72, ctrl_hum);
        i2c_reg_write_byte_dt(&bme688, 0x74, ctrl_meas);

        ret = i2c_burst_read_dt(&bme688, 0x22, temp_buf, 3);
        // if (ret < 0)
        // {
        //     LOG_ERR("I2C read failed (%d)", ret);
        // }

        temp_reading = ((uint32_t)temp_buf[0] << 12) | ((uint32_t)temp_buf[1] << 4) | ((uint32_t)temp_buf[2] >> 4);
        readings.temp = temp_reading;

        ret = i2c_burst_read_dt(&bme688, 0x25, hum_buf, 2);
        // if (ret < 0)
        // {
        //     LOG_ERR("I2C read failed (%d)", ret);
        // }

        humidity_reading = ((uint16_t)hum_buf[0] << 8) | ((uint16_t)hum_buf[1]);
        readings.hum = humidity_reading;

        // K_FOREVER allows me to wait until the message queue is not full to place something in it
        k_msgq_put(&sensor_queue, &readings, K_FOREVER);

        // TODO: I think I need to have threads sleep for them to actually switch between
        // I could also turn on tick rate and have the scheduler handle threads with the same priority
    }
}

void console_entry_point(void *a1, void *, void *)
{
    struct k_msgq *q = (struct kmsgq *)a1;
    struct sensor_readings values;

    while (true)
    {
        // might not want no wait here depending on how queue is being populated
        while (k_msgq_get(q, &values, K_NO_WAIT) == 0)
        {
            printk("temperature reading: %d\n", values.temp);
            printk("humidity reading: %d\n", values.hum);
        }

        k_msgq_purge(q);
    }
}

int main(void)
{
    // takes in the queue, buffer, size of the sturct, and the max number of messages allowed
    k_msgq_init(&sensor_queue, sensor_buffer, sizeof(struct sensor_readings), 10);

    // the inital count of this semaphore is 0, with the max number of threads that can take it being 1
    // k_sem_init(&sensor_read_sem, 0, 1);

    k_tid_t tid_1 = k_thread_create(&sensor_reading_thread,
                                    stack_area_1,
                                    K_THREAD_STACK_SIZEOF(stack_area_1),
                                    sensor_reading_entry_point,
                                    &sensor_queue, NULL, NULL,
                                    SENSOR_PRIO, 0, K_NO_WAIT);

    k_tid_t tid_2 = k_thread_create(&console_entry_point,
                                    stack_area_2,
                                    K_THREAD_STACK_SIZEOF(stack_area_2),
                                    console_entry_point,
                                    &sensor_queue, NULL, NULL,
                                    CONSOLE_PRIO, 0, K_NO_WAIT);

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
#include "sensor_thread.h"
#include <zephyr/drivers/i2c.h>

#define BME DT_NODELABEL(bme688)
static const struct i2c_dt_spec bme688 = I2C_DT_SPEC_GET(BME);

void sensor_reading_entry_point(void *a1, void *a2, void *a3)
{
    struct k_msgq *q = (struct k_msgq *)a1;
    struct bme688_readings readings;

    // configurations for the bme688
    uint8_t ctrl_hum = 0x01;
    uint8_t ctrl_meas = (0x02 << 5) | (0x01 << 2) | 0x01;

    uint8_t temp_buf[3];
    uint8_t hum_buf[2];
    uint8_t status;
    int ret;

    while (true)
    {
        // triggering forced mode (reads once then goes back to sleep)
        i2c_reg_write_byte_dt(&bme688, 0x72, ctrl_hum);
        i2c_reg_write_byte_dt(&bme688, 0x74, ctrl_meas);

        // wait for the measurement to finish reading from the sensor
        do
        {
            i2c_reg_read_byte_dt(&bme688, 0x1D, &status);
        } while (status & 0x08);

        // read temp and humidity
        ret = i2c_burst_read_dt(&bme688, 0x22, temp_buf, 3);
        ret = i2c_burst_read_dt(&bme688, 0x25, hum_buf, 2);

        readings.temp = ((uint32_t)temp_buf[0] << 12) | ((uint32_t)temp_buf[1] << 4) | ((uint32_t)temp_buf[2] >> 4);

        readings.hum = ((uint16_t)hum_buf[0] << 8) | ((uint16_t)hum_buf[1]);

        // put in message queue for the console thread to use for testing
        k_msgq_put(q, &readings, K_FOREVER);

        k_sleep(K_MSEC(100));
    }
}

void console_entry_point(void *a1, void *a2, void *a3)
{
    struct k_msgq *q = (struct k_msgq *)a1;
    struct bme688_readings values;

    while (true)
    {
        while (k_msgq_get(q, &values, K_NO_WAIT) == 0)
        {
            printk("temperature reading: %d\n", values.temp);
            printk("humidity reading: %d\n", values.hum);
        }
    }
}

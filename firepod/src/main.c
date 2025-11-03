// #include <stdio.h>
#include <zephyr/kernel.h>
// zephyr/device.h is included eventually through kernel.h
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/lora.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(bme688, LOG_LEVEL_DBG);

// This is accessing the DeviceTree node through its label (can see the entire devicetree in generated zephyr.dts file in the build directory)
#define LED2 DT_ALIAS(led0)
// can we do this?
// const DT_NODELABEL(BME688);
#define BME DT_NODELABEL(bme688)

// 'gpios' refers to the property name within the led0 node, which is itself inside the leds parent node in the devicetree
// you can see this^^ in code if you search for the 'zephyr/boards/nordic/nrf54l15dk/nrf54l15dk_common.dtsi' file
static const struct gpio_dt_spec led_2 = GPIO_DT_SPEC_GET(LED2, gpios);

static const struct i2c_dt_spec bme688 = I2C_DT_SPEC_GET(BME);

int main(void)
{

    // led_0.port is used because you need to check if the gpio controller itself is ready to be used, if you just do led_0.pin you will get a number like '9'
    if (!device_is_ready(led_2.port))
    {
        return 0;
    }

    int state = 0;

    int ret;
    // configuration of led_0 pin
    ret = gpio_pin_configure_dt(&led_2, GPIO_OUTPUT);

    if (ret != 0)
    {
        return 0;
    }

    if (!device_is_ready(bme688.bus))
    {
        printk("dooodooo %s daaadaaa", bme688.bus->name);
    }

    // confguring the bosch sensor
    uint8_t ctrl_hum = 0x01;
    uint8_t ctrl_meas = (0x02 << 5) | (0x01 << 2) | 0x01;

    uint8_t temp_buf[3];

    uint32_t temp_reading;

    uint8_t hum_buf[2];

    uint16_t humidity_reading;

    while (true)
    {
        i2c_reg_write_byte_dt(&bme688, 0x72, ctrl_hum);
        i2c_reg_write_byte_dt(&bme688, 0x74, ctrl_meas);

        ret = i2c_burst_read_dt(&bme688, 0x22, temp_buf, 3);
        if (ret < 0)
        {
            LOG_ERR("I2C read failed (%d)", ret);
        }

        temp_reading = ((uint32_t)temp_buf[0] << 12) | ((uint32_t)temp_buf[1] << 4) | ((uint32_t)temp_buf[2] >> 4);

        ret = i2c_burst_read_dt(&bme688, 0x25, hum_buf, 2);
        if (ret < 0)
        {
            LOG_ERR("I2C read failed (%d)", ret);
        }

        humidity_reading = ((uint16_t)hum_buf[0] << 8) | ((uint16_t)hum_buf[1]);

        // state = !state;
        // ret = gpio_pin_set_dt(&led_2, state);
        // if (ret != 0)
        // {
        //     return 0;
        // }
        // printk("LED is now in state: %d", state);

        printk("temperature reading: %d\n", temp_reading);

        printk("humidity reading: %d\n", humidity_reading);

        k_msleep(500);
    }

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
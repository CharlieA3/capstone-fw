#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <stdlib.h>

#include "sensor_thread.h"
#include "bme688_stubs.h"

#include "bosch/BME68x_SensorAPI/bme68x.h"
#include "bosch/BME68x_SensorAPI/bme68x_defs.h"

// K_MSGQ_DEFINE(bme688_queue, sizeof(struct bme688_readings), 10, 4);

// Bosch API device struct
static struct bme68x_dev bme;

void sensor_reading_entry_point(void *a1, void *a2, void *a3)
{
    // Using dependency injection here instead of statically defined queues. This allows for more modularity. Tradoff is memory security.
    struct k_msgq *q = (struct k_msgq *)a1;

    struct k_sem *lora_trigger_sem = (struct k_sem *)a2;

    init_bme688();
    set_config_bme688();

    while (1)
    {
        // triggers the sensor wake up every loop so we can take a measurement
        bme68x_set_op_mode(BME68X_FORCED_MODE, &bme);

        uint8_t n_fields = 0;
        struct bme68x_data raw[1];

        // check every 5 microseconds until the sensor is able to give data -> in forced mode (for power efficiency) the sensor starts from off, turns on, takes readings, sends them, turns back off
        do
        {
            bme.delay_us(5000, bme.intf_ptr);
            // uses the environmental compensation algorithms and gas-heating sequences that aren't attainable with regular i2c read functions
            bme68x_get_data(BME68X_FORCED_MODE, raw, &n_fields, &bme);
        } while (n_fields == 0);

        uint8_t status = raw[0].status;
        bool gas_valid = (status & BME68X_GASM_VALID_MSK) != 0;

        // converting float to fixed point integers so we don't have to use the FPU too much -> power hungry
        int32_t temp_mC = (int32_t)(raw[0].temperature * 1000.0f); // milli-celcius
        int32_t hum_mPct = (int32_t)(raw[0].humidity * 1000.0f);   // milli-percent
        int32_t pres_Pa = (int32_t)(raw[0].pressure);
        int32_t gas_cOhm = gas_valid ? (int32_t)(raw[0].gas_resistance * 100.0f) : -1; // centi-ohms

        // payload we want to send to the LoRa thread
        struct bme688_readings send_out_bme_data = {
            .temperature = temp_mC,
            .humidity = hum_mPct,
            .pressure = pres_Pa,
            .gas_resistance = gas_cOhm};

        printk("Temp: %d.%03d C\n", temp_mC / 1000, abs(temp_mC % 1000));
        printk("Hum : %d.%03d %%\n", hum_mPct / 1000, abs(hum_mPct % 1000));
        printk("Pres: %d Pa\n", pres_Pa);
        if (gas_cOhm >= 0)
        {
            printk("Gas : %d.%02d ohms\n", gas_cOhm / 100, gas_cOhm % 100);
        }
        else
        {
            printk("Gas : INVALID\n");
        }

        // put the data in the queue for the LoRa thread and give the semaphore
        k_msgq_put(q, &send_out_bme_data, K_NO_WAIT);

        k_sem_give(lora_trigger_sem);

        // sticking with this sleep here because we want to utilize deep sleep caused by tickless idle mode whenever possible to save power
        k_sleep(K_MSEC(5000));
    }
}

void init_bme688()
{
    // configure Bosch API -> function pointer assignment -> using glue functions to enable the use of Zephyr with this vendor API
    bme.intf = BME68X_I2C_INTF;
    bme.read = user_i2c_read;
    bme.write = user_i2c_write;
    bme.delay_us = user_delay_us;
    bme.intf_ptr = (void *)bme68x_get_i2c();

    const struct i2c_dt_spec *spec = bme68x_get_i2c();
    printk("[DBG] I2C addr=0x%02X bus=%s ready=%d\n", spec->addr, spec->bus->name, device_is_ready(spec->bus));

    // populates the bme struct with a bunch of calibration data
    int8_t rslt = bme68x_init(&bme);
    printk("[DBG] bme68x_init result=%d\n", rslt);

    if (rslt != BME68X_OK)
    {
        printk("[ERR] BME688 init failed: %d\n", rslt);
        return;
    }
}

void set_config_bme688()
{
    // configuration to determine how many measurements that will be requested and if there will be a filter applied
    struct bme68x_conf conf = {
        .os_hum = BME68X_OS_1X,
        .os_temp = BME68X_OS_2X,
        .os_pres = BME68X_OS_1X,
        .filter = BME68X_FILTER_OFF,
    };

    bme68x_set_conf(&conf, &bme);

    struct bme68x_heatr_conf heatr = {
        .enable = BME68X_ENABLE,
        .heatr_temp = 320,
        .heatr_dur = 150,
    };

    // ***** forced mode for getting multiple readings and not just one
    bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr, &bme);
}

void console_entry_point(void *a1, void *a2, void *a3)
{
    struct bme688_readings v;

    while (true)
    {
        k_msgq_get(&bme688_queue, &v, K_FOREVER);

        printk("\n[CONSOLE]\n");

        // temperature: milli-Celsius → X.YYY
        printk("Temp: %d.%03d C\n",
               v.temperature / 1000, abs(v.temperature % 1000));

        // humidity: milli-percent → X.YYY
        printk("Hum : %d.%03d %%\n",
               v.humidity / 1000, abs(v.humidity % 1000));

        // pressure: Pa
        printk("Press: %d Pa\n", v.pressure);

        // gas resistance: centi-ohm → X.YY
        if (v.gas_resistance >= 0)
        {
            printk("Gas : %d.%02d ohms\n",
                   v.gas_resistance / 100, abs(v.gas_resistance % 100));
        }
        else
        {
            printk("Gas : INVALID\n");
        }
    }
}
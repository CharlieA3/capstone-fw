// // #include <zephyr/drivers/i2c.h>
// // #include "sensor_thread.h"

// // #define BME DT_NODELABEL(bme688)
// // static const struct i2c_dt_spec bme688 = I2C_DT_SPEC_GET(BME);

// // void sensor_reading_entry_point(void *a1, void *a2, void *a3)
// // {
// //     struct k_msgq *q = (struct k_msgq *)a1;
// //     struct bme688_readings readings;

// //     // configurations for the bme688
// //     uint8_t ctrl_hum = 0x01;
// //     uint8_t ctrl_meas = (0x02 << 5) | (0x01 << 2) | 0x01;

// //     uint8_t temp_buf[3];
// //     uint8_t hum_buf[2];
// //     uint8_t status;
// //     int ret;

// //     while (true)
// //     {
// //         // triggering forced mode (reads once then goes back to sleep)
// //         i2c_reg_write_byte_dt(&bme688, 0x72, ctrl_hum);
// //         i2c_reg_write_byte_dt(&bme688, 0x74, ctrl_meas);

// //         // wait for the measurement to finish reading from the sensor
// //         do
// //         {
// //             i2c_reg_read_byte_dt(&bme688, 0x1D, &status);
// //         } while (status & 0x08);

// //         // read temp and humidity
// //         ret = i2c_burst_read_dt(&bme688, 0x22, temp_buf, 3);
// //         ret = i2c_burst_read_dt(&bme688, 0x25, hum_buf, 2);

// //         readings.temp = ((uint32_t)temp_buf[0] << 12) | ((uint32_t)temp_buf[1] << 4) | ((uint32_t)temp_buf[2] >> 4);

// //         readings.hum = ((uint16_t)hum_buf[0] << 8) | ((uint16_t)hum_buf[1]);

// //         // put in message queue for the console thread to use for testing
// //         k_msgq_put(q, &readings, K_FOREVER);

// //         k_sleep(K_MSEC(100));
// //     }
// // }

// // void console_entry_point(void *a1, void *a2, void *a3)
// // {
// //     struct k_msgq *q = (struct k_msgq *)a1;
// //     struct bme688_readings values;

// //     while (true)
// //     {
// //         while (k_msgq_get(q, &values, K_NO_WAIT) == 0)
// //         {
// //             printk("temperature reading: %d\n", values.temp);
// //             printk("humidity reading: %d\n", values.hum);
// //         }
// //     }
// // }



// // #include <zephyr/drivers/i2c.h>
// // #include "sensor_thread.h"
// // #include "bosch/BME68x_SensorAPI/bme68x.h"
// // #include "bme688_stubs.h"

// // static struct bme68x_dev bme;
// // extern const struct i2c_dt_spec *bme68x_get_i2c(void);


// // // void sensor_reading_entry_point(void *a1, void *a2, void *a3)
// // // {
// // //     struct k_msgq *q = (struct k_msgq *)a1;
// // //     struct bme688_data data;

// // //     // configurations for the bme688
// // //     uint8_t ctrl_hum = 0x01;                                // Set humidity sampling
// // //     uint8_t ctrl_meas = (0x02 << 5) | (0x01 << 2) | 0x01;   // Set temp samping + trigger measurement

// // //     uint8_t temp_buf[3];                                    // Buffer for 3 bytes used for temperature
// // //     uint8_t hum_buf[2];                                     // humidity
// // //     uint8_t gas_buf[3];
// // //     uint8_t status;
// // //     int ret;

// // //     while (true)
// // //     {
// // //         // triggering forced mode (reads once then goes back to sleep)
// // //         i2c_reg_write_byte_dt(&bme688, 0x72, ctrl_hum);
// // //         i2c_reg_write_byte_dt(&bme688, 0x74, ctrl_meas);

// // //         // wait for the measurement to finish reading from the sensor
// // //         do
// // //         {
// // //             i2c_reg_read_byte_dt(&bme688, 0x1D, &status);
// // //         } while (status & 0x08);

// // //         // read temp and humidity
// // //         ret = i2c_burst_read_dt(&bme688, 0x22, temp_buf, 3);
// // //         ret = i2c_burst_read_dt(&bme688, 0x25, hum_buf, 2);

// // //         /*
// // //         Temp data are 
// // //         */
// // //         data.temp = ((uint32_t)temp_buf[0] << 12) | ((uint32_t)temp_buf[1] << 4) | ((uint32_t)temp_buf[2] >> 4);

// // //         data.hum = ((uint16_t)hum_buf[0] << 8) | ((uint16_t)hum_buf[1]);

// // //         // put in message queue for the console thread to use for testing, wait forever if blocked until space in the queue opens up
// // //         k_msgq_put(q, &data, K_FOREVER);

// // //         k_sleep(K_MSEC(500));
// // //     }
// // // }

// // // void sensor_reading_entry_point(void *a1, void *a2, void *a3)
// // // {
// // //     struct k_msgq *q = (struct k_msgq *)a1;
// // //     struct bme688_data data;

// // //     /* --- Init Bosch API + debug I2C spec --- */
// // //     bme.intf      = BME68X_I2C_INTF;
// // //     bme.read      = user_i2c_read;
// // //     bme.write     = user_i2c_write;
// // //     bme.delay_us  = user_delay_us;
// // //     bme.intf_ptr  = (void *)bme68x_get_i2c();

// // //     const struct i2c_dt_spec *spec = bme68x_get_i2c();
// // //     printk("[DBG] I2C addr = 0x%02X bus=%s ready=%d\n",
// // //         spec->addr, spec->bus->name, device_is_ready(spec->bus));

// // //     printk("[DBG] &bme struct = %p\n", (void *)&bme);
// // //     printk("[DBG] intf_ptr = %p\n", bme.intf_ptr);
// // //     printk("[DBG] thread id = %p\n", (void *)k_current_get());

// // //     int8_t rslt = bme68x_init(&bme);
// // //     printk("[DBG] bme68x_init result = %d\n", rslt);
// // //     if (rslt != BME68X_OK) {
// // //         printk("[ERR] BME688 init failed: %d\n", rslt);
// // //         return;
// // //     }

// // //     /* --- EXACT MATCH to your working register settings --- */
// // //     struct bme68x_conf conf = {
// // //         .os_hum  = BME68X_OS_1X,    // ctrl_hum = 0x01
// // //         .os_temp = BME68X_OS_2X,    // temp 2x (same as your ctrl_meas)
// // //         .os_pres = BME68X_OS_1X,    // pressure 1x (matches ctrl_meas)
// // //         .filter  = BME68X_FILTER_OFF
// // //     };
// // //     rslt = bme68x_set_conf(&conf, &bme);
// // //     printk("[DBG] bme68x_set_conf result = %d\n", rslt);

// // //     struct bme68x_heatr_conf heatr = {
// // //         .enable     = BME68X_DISABLE,
// // //         .heatr_temp = 0,
// // //         .heatr_dur  = 0
// // //     };
// // //     rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr, &bme);
// // //     printk("[DBG] bme68x_set_heatr_conf result = %d\n", rslt);

// // //     while (true)
// // //     {
// // //         /* --- Trigger forced measurement (equivalent to writing ctrl_meas forced) --- */
// // //         rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme);
// // //         printk("[DBG] set_op_mode result = %d\n", rslt);

// // //         /* --- Poll until data is available --- */
// // //         uint8_t n_fields = 0;
// // //         struct bme68x_data comp_data[1];

// // //         do {
// // //             bme.delay_us(5000, bme.intf_ptr);  // 5ms polling
// // //             rslt = bme68x_get_data(BME68X_FORCED_MODE, comp_data, &n_fields, &bme);
// // //             if (rslt != BME68X_OK && rslt != BME68X_OK) {
// // //                 printk("[ERR] bme68x_get_data returned %d\n", rslt);
// // //             }
// // //         } while (n_fields == 0);

// // //         /* --- Copy compensated values into your queue struct --- */
// // //         data.temp = comp_data[0].temperature;
// // //         data.hum  = comp_data[0].humidity;
// // //         data.gas  = comp_data[0].gas_resistance; // optional if comp_data provides

// // //         /* --- Push into queue --- */
// // //         k_msgq_put(q, &data, K_FOREVER);

// // //         /* --- Sampling delay (your same 500 ms) --- */
// // //         k_sleep(K_MSEC(500));
// // //     }
// // // }

// // void sensor_reading_entry_point(void *a1, void *a2, void *a3)
// // {
// //     struct k_msgq *q = (struct k_msgq *)a1;
// //     struct bme688_data data;

// //     /* --- Init Bosch API + debug I2C spec --- */
// //     bme.intf      = BME68X_I2C_INTF;
// //     bme.read      = user_i2c_read;
// //     bme.write     = user_i2c_write;
// //     bme.delay_us  = user_delay_us;
// //     bme.intf_ptr  = (void *)bme68x_get_i2c();

// //     const struct i2c_dt_spec *spec = bme68x_get_i2c();
// //     printk("[DBG] I2C addr = 0x%02X bus=%s ready=%d\n",
// //         spec->addr, spec->bus->name, device_is_ready(spec->bus));

// //     printk("[DBG] &bme struct = %p\n", (void *)&bme);
// //     printk("[DBG] intf_ptr = %p\n", bme.intf_ptr);
// //     printk("[DBG] thread id = %p\n", (void *)k_current_get());

// //     int8_t rslt = bme68x_init(&bme);
// //     printk("[DBG] bme68x_init result = %d\n", rslt);
// //     if (rslt != BME68X_OK) {
// //         printk("[ERR] BME688 init failed: %d\n", rslt);
// //         return;
// //     }

// //     /* --- Your working oversampling settings --- */
// //     struct bme68x_conf conf = {
// //         .os_hum  = BME68X_OS_1X,
// //         .os_temp = BME68X_OS_2X,
// //         .os_pres = BME68X_OS_1X,
// //         .filter  = BME68X_FILTER_OFF
// //     };
// //     rslt = bme68x_set_conf(&conf, &bme);
// //     printk("[DBG] bme68x_set_conf result = %d\n", rslt);

// //     /* --- ENABLE HEATER --- */
// //     struct bme68x_heatr_conf heatr = {
// //         .enable      = BME68X_ENABLE,
// //         .heatr_temp  = 300,   // 300°C heater (Bosch recommended starting point)
// //         .heatr_dur   = 100    // 100 ms heating duration
// //     };

// //     rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr, &bme);
// //     printk("[DBG] bme68x_set_heatr_conf result = %d\n", rslt);

// //     while (true)
// //     {
// //         /* Trigger a forced measurement */
// //         rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme);

// //         /* Poll until data is available */
// //         uint8_t n_fields = 0;
// //         struct bme68x_data comp_data[1];

// //         do {
// //             bme.delay_us(5000, bme.intf_ptr);
// //             rslt = bme68x_get_data(BME68X_FORCED_MODE, comp_data, &n_fields, &bme);
// //         } while (n_fields == 0);

// //         /* Read status bits */
// //         uint8_t status = comp_data[0].status;
// //         bool gas_valid = (status & BME68X_GASM_VALID_MSK) != 0;
// //         bool heat_stab = (status & BME68X_HEAT_STAB_MSK) != 0;

// //         printk("[DBG] gas_valid=%d heat_stab=%d\n", gas_valid, heat_stab);

// //         /* Copy sensor values */
// //         data.temp = comp_data[0].temperature;
// //         data.hum  = comp_data[0].humidity;
// //         data.gas  = comp_data[0].gas_resistance;   // <-- GAS HERE

// //         uint32_t pres_pa = (uint32_t) comp_data[0].pressure; // Pascals
// //         data.pres = pres_pa;

// //         /* print values (integer-safe) */
// //         /* temperature/humidity: you already print integer versions elsewhere; if using fixed-point convert accordingly */
// //         printk("temperature: %d C\n", data.temp);   // if your temp is integer degrees; if temp is scaled (x100) use conversion shown earlier
// //         printk("humidity: %d %%\n", data.hum);

// //         /* Print pressure in Pa and hPa with two decimal digits (integer-only) */
// //         uint32_t pres_hPa = pres_pa / 100;
// //         uint32_t pres_hPa_frac = pres_pa % 100; /* remainder Pa -> printed as two digits */
// //         printk("pressure: %u Pa (%u.%02u hPa)\n", pres_pa, pres_hPa, pres_hPa_frac);

// //         /* Print gas if valid (unchanged) */
// //         if (gas_valid && heat_stab) {
// //             /* gas is already in data.gas */
// //             uint32_t gas_whole = (uint32_t)data.gas;
// //             printk("gas resistance: %u ohms\n", gas_whole);
// //         } else {
// //             printk("gas resistance: invalid\n");
// //         }

// //         /* Push to queue */
// //         k_msgq_put(q, &data, K_FOREVER);

// //         k_sleep(K_MSEC(500));
// //     }
// // }

// // void console_entry_point(void *a1, void *a2, void *a3)
// // {
// //     struct k_msgq *q = (struct k_msgq *)a1;
// //     struct bme688_data values;

// //     while (true)
// //     {
// //         while (k_msgq_get(q, &values, K_NO_WAIT) == 0)  // k_msgq_get removes values from the queue
// //         {
// //             printk("temperature reading: %d\n", values.temp);
// //             printk("humidity reading: %d\n", values.hum);
// //         }
// //     }
// // }


#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

#include "sensor_thread.h"
#include "bme688_stubs.h"

#include "bosch/BME68x_SensorAPI/bme68x.h"
#include "bosch/BME68x_SensorAPI/bme68x_defs.h"

// K_MSGQ_DEFINE(bme688_queue, sizeof(struct bme688_readings), 10, 4);

// Bosch API device struct
static struct bme68x_dev bme;


// void sensor_reading_entry_point(void *a1, void *a2, void *a3)
// {
//     struct k_msgq *q = (struct k_msgq *)a1;

//     /* --- Configure Bosch API --- */
//     bme.intf      = BME68X_I2C_INTF;
//     bme.read      = user_i2c_read;
//     bme.write     = user_i2c_write;
//     bme.delay_us  = user_delay_us;
//     bme.intf_ptr  = (void *)bme68x_get_i2c();

//     const struct i2c_dt_spec *spec = bme68x_get_i2c();
//     printk("[DBG] I2C addr=0x%02X bus=%s ready=%d\n",
//            spec->addr, spec->bus->name, device_is_ready(spec->bus));

//     int8_t rslt = bme68x_init(&bme);
//     printk("[DBG] bme68x_init result=%d\n", rslt);
//     if (rslt != BME68X_OK) {
//         printk("[ERR] BME688 init failed: %d\n", rslt);
//         return;
//     }

//     /* --- Sensor configuration (matches your working manual register config) --- */
//     struct bme68x_conf conf = {
//         .os_hum  = BME68X_OS_1X,
//         .os_temp = BME68X_OS_2X,
//         .os_pres = BME68X_OS_1X,
//         .filter  = BME68X_FILTER_OFF,
//     };

//     rslt = bme68x_set_conf(&conf, &bme);
//     printk("[DBG] set_conf=%d\n", rslt);

//     /* --- Enable gas + heater --- */
//     struct bme68x_heatr_conf heatr = {
//         .enable     = BME68X_ENABLE,
//         .heatr_temp = 320,   // °C typical detection setting
//         .heatr_dur  = 150,   // ms heater-on duration
//     };

//     rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr, &bme);
//     printk("[DBG] set_heatr_conf=%d\n", rslt);

//     /* --- Main loop --- */
//     while (true) {

//         /* Trigger forced measurement */
//         rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme);
//         printk("[DBG] set_op_mode=%d\n", rslt);

//         uint8_t n_fields = 0;
//         struct bme68x_data raw[1];

//         /* Poll until data becomes available */
//         do {
//             bme.delay_us(5000, bme.intf_ptr);
//             rslt = bme68x_get_data(BME68X_FORCED_MODE, raw, &n_fields, &bme);
//         } while (n_fields == 0);

//         uint8_t status = raw[0].status;
//         bool gas_valid = (status & BME68X_GASM_VALID_MSK) != 0;
//         bool heat_stab = (status & BME68X_HEAT_STAB_MSK) != 0;

//         printk("[DBG] gas_valid=%d heat_stab=%d\n", gas_valid, heat_stab);

//         struct bme688_readings out = {
//             .temperature     = raw[0].temperature,
//             .humidity        = raw[0].humidity,
//             .pressure        = raw[0].pressure,
//             .gas_resistance  = gas_valid ? raw[0].gas_resistance : -1.0f
//         };

//         printk("temperature: %.2d C\n", out.temperature);
//         printk("humidity: %.3d %%\n", out.humidity);
//         printk("pressure: %.2d Pa\n", out.pressure);
//         printk("gas: %.2d ohms\n", out.gas_resistance);

//         k_msgq_put(q, &out, K_NO_WAIT);

//         k_sleep(K_MSEC(500));
//     }
// }

void sensor_reading_entry_point(void *a1, void *a2, void *a3)
{
    struct k_msgq *q = (struct k_msgq *)a1;

    /* --- Configure Bosch API --- */
    bme.intf      = BME68X_I2C_INTF;
    bme.read      = user_i2c_read;
    bme.write     = user_i2c_write;
    bme.delay_us  = user_delay_us;
    bme.intf_ptr  = (void *)bme68x_get_i2c();

    const struct i2c_dt_spec *spec = bme68x_get_i2c();
    printk("[DBG] I2C addr=0x%02X bus=%s ready=%d\n",
           spec->addr, spec->bus->name, device_is_ready(spec->bus));

    int8_t rslt = bme68x_init(&bme);
    printk("[DBG] bme68x_init result=%d\n", rslt);
    if (rslt != BME68X_OK) {
        printk("[ERR] BME688 init failed: %d\n", rslt);
        return;
    }

    /* --- Sensor config --- */
    struct bme68x_conf conf = {
        .os_hum  = BME68X_OS_1X,
        .os_temp = BME68X_OS_2X,
        .os_pres = BME68X_OS_1X,
        .filter  = BME68X_FILTER_OFF,
    };

    bme68x_set_conf(&conf, &bme);

    struct bme68x_heatr_conf heatr = {
        .enable     = BME68X_ENABLE,
        .heatr_temp = 320,
        .heatr_dur  = 150,
    };

    bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr, &bme);

    /* --- MAIN LOOP --- */
    while (1) {

        bme68x_set_op_mode(BME68X_FORCED_MODE, &bme);

        uint8_t n_fields = 0;
        struct bme68x_data raw[1];

        do {
            bme.delay_us(5000, bme.intf_ptr);
            bme68x_get_data(BME68X_FORCED_MODE, raw, &n_fields, &bme);
        } while (n_fields == 0);

        uint8_t status = raw[0].status;
        bool gas_valid = (status & BME68X_GASM_VALID_MSK) != 0;

        /* ---- Convert float → fixed point ints ---- */

        int32_t temp_mC   = (int32_t)(raw[0].temperature * 1000.0f);
        int32_t hum_mPct  = (int32_t)(raw[0].humidity * 1000.0f);
        int32_t pres_Pa   = (int32_t)(raw[0].pressure);
        int32_t gas_cOhm  = gas_valid ? (int32_t)(raw[0].gas_resistance * 100.0f) : -1;

        struct bme688_readings out = {
            .temperature = temp_mC,
            .humidity  = hum_mPct,
            .pressure    = pres_Pa,
            .gas_resistance       = gas_cOhm
        };

        /* ---- Print nicely formatted values ---- */

        printk("Temp: %d.%03d C\n",
               temp_mC / 1000, abs(temp_mC % 1000));

        printk("Hum : %d.%03d %%\n",
               hum_mPct / 1000, abs(hum_mPct % 1000));

        printk("Pres: %d Pa\n",
               pres_Pa);

        if (gas_cOhm >= 0) {
            printk("Gas : %d.%02d ohms\n",
                   gas_cOhm / 100, gas_cOhm % 100);
        } else {
            printk("Gas : INVALID\n");
        }

        /* Send to message queue */
        k_msgq_put(q, &out, K_NO_WAIT);

        k_sleep(K_MSEC(500));
    }
}



// void console_entry_point(void *a1, void *a2, void *a3)
// {
//     struct bme688_readings v;

//     while (true) {
//         k_msgq_get(&bme688_queue, &v, K_FOREVER);

//         printk("\n[CONSOLE]\n");
//         printk("Temp: %.2f C\n", v.temperature);
//         printk("Hum : %.3f %%\n", v.humidity);
//         printk("Press: %.2f Pa\n", v.pressure);
//         printk("Gas : %.2f ohms\n", v.gas_resistance);
//     }
// }

void console_entry_point(void *a1, void *a2, void *a3)
{
    struct bme688_readings v;

    while (true) {
        k_msgq_get(&bme688_queue, &v, K_FOREVER);

        printk("\n[CONSOLE]\n");

        // Temperature: milli-Celsius → X.YYY
        printk("Temp: %d.%03d C\n",
               v.temperature / 1000, abs(v.temperature % 1000));

        // Humidity: milli-percent → X.YYY
        printk("Hum : %d.%03d %%\n",
               v.humidity / 1000, abs(v.humidity % 1000));

        // Pressure: Pa (already int)
        printk("Press: %d Pa\n", v.pressure);

        // Gas resistance: centi-ohm → X.YY
        if (v.gas_resistance >= 0) {
            printk("Gas : %d.%02d ohms\n",
                   v.gas_resistance / 100, abs(v.gas_resistance % 100));
        } else {
            printk("Gas : INVALID\n");
        }
    }
}
#ifndef BME688_STUBS_H
#define BME688_STUBS_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include "bosch/BME68x_SensorAPI/bme68x.h"   // needed for bme68x types

int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
void   user_delay_us(uint32_t period, void *intf_ptr);

const struct i2c_dt_spec *bme68x_get_i2c(void);

#endif

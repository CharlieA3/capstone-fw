#include "bme688_stubs.h"

#define BME DT_NODELABEL(bme688)

static const struct i2c_dt_spec bme688 = I2C_DT_SPEC_GET(BME);

int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    const struct i2c_dt_spec *i2c = (const struct i2c_dt_spec *)intf_ptr;

    int ret = i2c_burst_read_dt(i2c, reg_addr, reg_data, length);

    printk("[STUB] READ reg=0x%02X len=%d ret=%d\n", reg_addr, length, ret);

    return (ret == 0) ? 0 : -1;
}

int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    const struct i2c_dt_spec *i2c = (const struct i2c_dt_spec *)intf_ptr;

    int ret = i2c_burst_write_dt(i2c, reg_addr, reg_data, length);

    printk("[STUB] WRITE reg=0x%02X len=%d ret=%d\n", reg_addr, length, ret);

    return (ret == 0) ? 0 : -1;
}

void user_delay_us(uint32_t period, void *intf_ptr)
{
    k_busy_wait(period);
}

const struct i2c_dt_spec *bme68x_get_i2c(void)
{
    if (!device_is_ready(bme688.bus)) {
        printk("BME688 I2C bus not ready!\n");
    }
    return &bme688;
}

#include <stdio.h>
#include <zephyr/kernel.h>
// #include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define SYS_CLOCK_HW_CYCLES_PER_SEC CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int main(void)
{
    if (!device_is_ready(&led))
    {
        return 0;
    }

    int state = 0;

    int ret;
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);

    if (ret != 0)
    {
        return 0;
    }

    while (true)
    {
        state = !state;
        ret = gpio_pin_set_dt(&led, state);
        if (ret != 0)
        {
            return 0;
        }
        printk("LED is now in state: %d", state);

        k_msleep(1000);
    }

    return 0;
}
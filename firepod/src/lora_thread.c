#include <zephyr/drivers/spi.h>
#include "lora_thread.h"

// access the node from the devicetree using SPI
#define SX1262 DT_NODELABEL(sx1262)
static const struct spi_dt_spec sx1262 = SPI_DT_SPEC_GET(SX1262, SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER, 0);

void lora_thread(void *a1, void *a2, void *a3)
{
    // first configure the lora device using something like this
    /*
    struct lora_modem_config config = {
    .frequency = 915000000,
    .bandwidth = BW_125_KHZ,
    .datarate = SF_7,
    .coding_rate = CR_4_5,
    .preamble_len = 8,
    .tx_power = 14,
    .tx = false,   // this field is ignored now
    };

    lora_config(lora_dev, &config);
    */

    while (true)
    {
        /*
        1. format the data taken from message queue that gets posted to by the i2c thread into a payload
        2. use lora_send to send payload (DIO 1 is handled under the hood by hte sx1262 drivers)
        */

        bool ret = check_heartbeat();

        printk("heartbeat: %d\n", (int)ret);
        k_sleep(K_MSEC(1000));
    }
}

bool check_heartbeat()
{
    return spi_is_ready_dt(&sx1262);
}
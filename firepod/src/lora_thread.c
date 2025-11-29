#include <zephyr/drivers/spi.h>
#include "lora_thread.h"

// access the node from the devicetree using SPI
#define SX1262 DT_NODELABEL(sx1262)
static const struct spi_dt_spec sx1262 = SPI_DT_SPEC_GET(SX1262, SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER, 0);

void lora_thread(void *a1, void *a2, void *a3)
{
    while (true)
    {
        bool ret = check_heartbeat();

        printk("heartbeat: %d\n", (int)ret);
        k_sleep(K_MSEC(1000));
    }
}

bool check_heartbeat()
{
    return spi_is_ready_dt(&sx1262);
}
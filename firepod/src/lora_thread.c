#include "lora_thread.h"

// access the node from the devicetree using SPI
#define SX1262 DT_NODELABEL(sx1262)
static const struct spi_dt_spec sx1262 = SPI_DT_SPEC_GET(SX1262);

void lora_thread(void *a1, void *a2, void *a3)
{
    printk("something goes here");
}
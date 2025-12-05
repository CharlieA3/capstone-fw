#include <zephyr/drivers/lora.h>
#include <zephyr/drivers/spi.h>
#include "lora_thread.h"

#define NO_DATA 0
#define POD_1 1

// Access the node from the devicetree using SPI
#define SX1262 DT_NODELABEL(sx1262)
static const struct spi_dt_spec sx1262 = SPI_DT_SPEC_GET(SX1262, SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER, 0);

// Get the LoRa device from the same node
static const struct device *lora_dev = DEVICE_DT_GET(SX1262);

void lora_thread_entry_point(void *a1, void *a2, void *a3)
{
    int ret;
    spi_sx1262_packet_t sensor_data_send = {NO_DATA};

    struct k_msgq *sx1262_queue = (struct k_msgq *)a1;

    if (!device_is_ready(lora_dev))
    {
        printk("LoRa device not ready\n");
        return;
    }

    struct lora_modem_config config = {
        .frequency = 915000000,
        .bandwidth = BW_125_KHZ,
        .datarate = SF_7,
        .coding_rate = CR_4_7,
        .preamble_len = 8,
        .tx_power = 14,
        .tx = true,
        .public_network = true};

    ret = lora_config(lora_dev, &config);
    if (ret < 0)
    {
        printk("LoRa config failed: %d\n", ret);
        return;
    }

    sensor_data_send.identifier = POD_1;

    while (true)
    {
        // this will populate the sensor_data_send packet struct
        if (k_msgq_get(&sx1262_queue, &sensor_data_send, K_NO_WAIT) == 0)
        {
            ret = lora_send(lora_dev, (uint8_t *)&sensor_data_send, sizeof(sensor_data_send));

            (ret < 0) ? printk("LoRa send failed: %d\n", ret) : printk("Sent %d bytes over LoRa\n", sizeof(sensor_data_send));
        }
        else
        {
            // send NO_DATA if there is nothing in the queue (we still want to indicate there is not data which is why we still sending)
            sensor_data_send.temperature = NO_DATA;
            sensor_data_send.humidity = NO_DATA;
            sensor_data_send.pressure = NO_DATA;
            sensor_data_send.gas_resistance = NO_DATA;

            ret = lora_send(lora_dev, &sensor_data_send, sizeof(sensor_data_send));

            (ret < 0) ? printk("LoRa send failed: %d\n", ret) : printk("Sent test message\n");
        }
    }
}

bool check_heartbeat()
{
    return device_is_ready(lora_dev);
}
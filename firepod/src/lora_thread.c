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
    spi_sx1262_packet_t sensor_data_send = {0};
    only_sensor_data_packet_t only_sensor_data = {0};

    printk("Entered lora thread");

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
        .coding_rate = CR_4_5,
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

    struct k_sem *lora_trigger_sem = (struct k_sem *)a2;

    while (true)
    {
        printk("Inside while loop of lora thread");
        k_sem_take(lora_trigger_sem, K_FOREVER);

        // this will populate the sensor_data_send packet struct
        if (k_msgq_get(sx1262_queue, &only_sensor_data, K_NO_WAIT) == 0)
        {

            sensor_data_send.temperature = only_sensor_data.temperature;
            sensor_data_send.gas_resistance = only_sensor_data.gas_resistance;
            sensor_data_send.humidity = only_sensor_data.humidity;
            sensor_data_send.pressure = only_sensor_data.pressure;

            printk("Raw packet (%d bytes): ", sizeof(sensor_data_send));
            uint8_t *raw_bytes = (uint8_t *)&sensor_data_send;
            for (int i = 0; i < sizeof(sensor_data_send); i++)
            {
                printk("%02X ", raw_bytes[i]);
            }
            printk("\n");

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

            ret = lora_send(lora_dev, (uint8_t *)&sensor_data_send, sizeof(sensor_data_send));

            (ret < 0) ? printk("LoRa send failed: %d\n", ret) : printk("Sent test message\n");
        }
    }
}

bool check_heartbeat()
{
    return device_is_ready(lora_dev);
}
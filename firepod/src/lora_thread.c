#include <zephyr/drivers/lora.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/reboot.h>
#include "lora_thread.h"

#define NO_DATA 0
#define POD_1 1
#define ROC_THRESHOLD 2 // 2 degree delta
#define GAS_THRESHOLD 50000
#define TEMP_MAX 45

// Access the node from the devicetree
#define SX1262 DT_NODELABEL(sx1262)

// if the low level spi driver was necessary instead of the high level LoRa driver, then I would use this line below
// static const struct spi_dt_spec sx1262 = SPI_DT_SPEC_GET(SX1262, SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER);

// Get the LoRa device from the same node
static const struct device *const lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));

static uint8_t run_fire_algorithm(bme688_data_packet_t *data)
{
    // static so it can be used to compare
    static int32_t last_temp = 0;
    uint8_t score = 0;

    if (data->gas_resistance < GAS_THRESHOLD)
        score += 2;
    if (data->temperature > TEMP_MAX)
        score += 1;

    // only check rate of change will multiple data points
    if (last_temp != 0)
    {
        int32_t delta_t = data->temperature - last_temp;

        if (delta_t >= ROC_THRESHOLD)
        {
            printk("[ALGO] Rapid Temp Rise Detected: +%d C\n", delta_t);
            score += 2;
        }
    }

    // update the "last" value for the next time the thread runs
    last_temp = data->temperature;

    // return: 0 (cold), 1 (hot), 2 (smoky/dangerous), 3-5 (fire)
    return score;
}

/* NOTE:
 - If this needed to send with a high frequency, printing should be offloaded to a work queue because the operation is slow -> it sends message character by character over UART
 - Another option is enabling CONFIG_LOG_MODE_DEFERRED=y to delay the log printing by storing it in a buffer and prints when the CPU is idle
*/
void lora_thread_entry_point(void *a1, void *a2, void *a3)
{
    int ret;
    spi_sx1262_packet_t lora_packet_tx = {0};
    bme688_data_packet_t bme688_data = {0};

    printk("Entered lora thread");

    bool init_complete = init_lora_node();
    if (!init_complete)
    {
        printk("FATAL: LoRa init failed. Hardware may be unresponsive.\n");
        printk("Rebooting system in 10 seconds to attempt recovery...\n");

        // Give the user time to read the message over UART before the reboot
        k_sleep(K_SECONDS(10));

        // This triggers a hardware-level reset of the microcontroller
        sys_reboot(SYS_REBOOT_COLD);
    }

    struct k_msgq *sx1262_queue = (struct k_msgq *)a1;

    // in a network of sensors, this will be passed in to the thread
    lora_packet_tx.identifier = POD_1;

    struct k_sem *lora_trigger_sem = (struct k_sem *)a2;

    while (true)
    {
        printk("Inside while loop of lora thread");
        k_sem_take(lora_trigger_sem, K_FOREVER);

        // this will populate the sensor_data_send packet struct
        if (k_msgq_get(sx1262_queue, &bme688_data, K_NO_WAIT) == 0)
        {

            lora_packet_tx.env_data = bme688_data;

            lora_packet_tx.fire_alert = run_fire_algorithm(&bme688_data);

            // great example of code that can be offloaded to a work queue
            printk("Raw packet (%d bytes): ", sizeof(lora_packet_tx));
            uint8_t *raw_bytes = (uint8_t *)&lora_packet_tx;
            for (int i = 0; i < sizeof(lora_packet_tx); i++)
            {
                printk("%02X ", raw_bytes[i]);
            }
            printk("\n");

            ret = lora_send(lora_dev, (uint8_t *)&lora_packet_tx, sizeof(lora_packet_tx));

            (ret < 0) ? printk("LoRa send failed: %d\n", ret) : printk("Sent %d bytes over LoRa\n", sizeof(lora_packet_tx));
        }
        else
        {
            // this shouldn't be hit because of how the semaphore works, but there could be a bug where the queue was empty, then this might be reached
            printk("[WARN] LoRa triggered but queue was empty.\n");
        }
    }
}

bool init_lora_node()
{
    int ret;
    if (!device_is_ready(lora_dev))
    {
        printk("LoRa device not ready\n");
        return false;
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
        return false;
    }
    return true;
}

bool check_heartbeat()
{
    return device_is_ready(lora_dev);
}
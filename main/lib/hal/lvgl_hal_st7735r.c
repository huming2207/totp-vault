#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <lvgl.h>

#include "lvgl_hal_st7735r.h"

#define  LOG_TAG "st7735"

// M5StickC's ST7735R 80*160 panel memory region offset
#define ST7735_M5C_COL_OFFSET 26
#define ST7735_M5C_ROW_OFFSET 1

// ST7735 specific commands used in init
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B // PASET
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
//Add
#define ST7735_VSCRDEF 0x33
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
#define ST7735_VSCRSADD 0x37

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

static spi_device_handle_t device_handle;

typedef struct {
    uint8_t reg;
    uint8_t data[16];
    uint8_t len;
} st7735s_init_t;

/**
 * From: https://github.com/m5stack/M5StickC/blob/2431efeff0c65f4c6ac7b26a1c9827d4598b7950/src/utility/ST7735_Init.h
 *       https://github.com/adafruit/Adafruit-ST7735-Library/blob/master/Adafruit_ST7735.cpp
 * Combination of Rcmd1 + Rcmd2Green + Rcmd3
 */
static const st7735s_init_t st7735s_init_seq[] = {
    { ST7735_FRMCTR1, { 0x01, 0x2c, 0x2d }, 3 },
    { ST7735_FRMCTR2, { 0x01, 0x2c, 0x2d }, 3 },
    { ST7735_FRMCTR3, { 0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d }, 6 },
    { ST7735_INVCTR, { 0x07 }, 1 },
    { ST7735_PWCTR1, { 0xa2, 0x02, 0x84 }, 3 }, // Power control #1: 4.6v, auto
    { ST7735_PWCTR2, { 0xc5 }, 1 }, // Power control #2
    { ST7735_PWCTR3, { 0x0a, 0x00 }, 2 },
    { ST7735_PWCTR4, { 0x8a, 0x2a }, 2 },
    { ST7735_PWCTR5, { 0x8a, 0xee }, 2 },
    { ST7735_VMCTR1, { 0x0e }, 1 },
    { ST7735_INVON, {  }, 0 },
    { ST7735_MADCTL, { 0x08 }, 1 }, // Normal memory write order, BGR filter panel
    { ST7735_COLMOD, { 0x05 }, 1 }, // 16 bit RGB
    { ST7735_CASET,  { 0x00, 0x00, 0x00, 0x4f }, 4 },
    { ST7735_RASET,  { 0x00, 0x00, 0x00, 0x9f }, 4 },
    { ST7735_GMCTRP1, {
        0x02, 0x1c, 0x07, 0x12,
        0x37, 0x32, 0x29, 0x2d,
        0x29, 0x25, 0x2B, 0x39,
        0x00, 0x01, 0x03, 0x10,
    }, 16 },
    { ST7735_GMCTRN1, {
        0x03, 0x1d, 0x07, 0x06,
        0x2E, 0x2C, 0x29, 0x2D,
        0x2E, 0x2E, 0x37, 0x3F,
        0x00, 0x00, 0x02, 0x10,
    }, 16 },
};

static void st7735r_spi_send_bytes(const uint8_t *payload, size_t len, bool is_cmd)
{
    if(!payload) {
        ESP_LOGE(LOG_TAG, "Payload is null!");
        return;
    }

    spi_transaction_t spi_tract;
    memset(&spi_tract, 0, sizeof(spi_tract));

    spi_tract.tx_buffer = payload;
    spi_tract.length = len * 8;
    spi_tract.rxlength = 0;

    // ESP_LOGD(LOG_TAG, "Sending SPI payload, length : %d, is_cmd: %s", len, is_cmd ? "TRUE" : "FALSE");
    ESP_ERROR_CHECK(gpio_set_level(CONFIG_LVGL_IO_DC, is_cmd ? 0 : 1)); // Low for CMD, High for DATA
    ESP_ERROR_CHECK(spi_device_polling_transmit(device_handle, &spi_tract)); // Use blocking transmit for now (easier to debug)
    // ESP_LOGD(LOG_TAG, "SPI payload sent!");
}

static void st7735r_spi_send_cmd(uint8_t payload)
{
    st7735r_spi_send_bytes(&payload, 1, true);
}

static void st7735r_send_init_seq(const st7735s_init_t *seq)
{
    if(!seq) {
        ESP_LOGE(LOG_TAG, "Sequence is null!");
        return;
    }

    ESP_LOGD(LOG_TAG, "Writing to register 0x%02X, length: %u", seq->reg, seq->len);
    st7735r_spi_send_bytes(&seq->reg, 1, true);

    if(seq->len > 0) {
        st7735r_spi_send_bytes(seq->data, seq->len, false);
    }
}

static void st7735r_set_addr_window(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
    x1 += ST7735_M5C_COL_OFFSET;
    x2 += ST7735_M5C_COL_OFFSET;
    y1 += ST7735_M5C_ROW_OFFSET;
    y2 += ST7735_M5C_ROW_OFFSET;

    uint8_t x_start[] = {(uint8_t)(x1 >> 8u), (uint8_t)(x1 & 0xffU)};
    uint8_t x_end[] = {(uint8_t)(x2 >> 8u), (uint8_t)(x2 & 0xffU)};
    uint8_t y_start[] = {(uint8_t)(y1 >> 8u), (uint8_t)(y1 & 0xffU)};
    uint8_t y_end[] = {(uint8_t)(y2 >> 8u), (uint8_t)(y2 & 0xffU)};

    ESP_LOGD(LOG_TAG, "Setting position in: "
                      "x1: 0x%02X, x2: 0x%02X; y1: 0x%02X, y2: 0x%02X", x1, x2, y1, y2);

    st7735r_spi_send_cmd(ST7735_CASET);
    st7735r_spi_send_bytes(x_start, 2, false);
    st7735r_spi_send_bytes(x_end, 2, false);

    st7735r_spi_send_cmd(ST7735_RASET);
    st7735r_spi_send_bytes(y_start, 2, false);
    st7735r_spi_send_bytes(y_end, 2, false);

    ESP_LOGD(LOG_TAG, "Position set!");
}

static void st7735r_spi_send_pixel(const uint16_t *payload, size_t len)
{
    if(!payload) {
        ESP_LOGE(LOG_TAG, "Payload is null!");
        return;
    }

    spi_transaction_t spi_tract;
    memset(&spi_tract, 0, sizeof(spi_tract));

    spi_tract.tx_buffer = payload;
    spi_tract.length = len * 16;
    spi_tract.rxlength = 0;

    // ESP_LOGD(LOG_TAG, "Sending SPI payload, length : %d, is_cmd: %s", len, is_cmd ? "TRUE" : "FALSE");
    ESP_ERROR_CHECK(gpio_set_level(CONFIG_LVGL_IO_DC, 1));
    ESP_ERROR_CHECK(spi_device_polling_transmit(device_handle, &spi_tract)); // Use blocking transmit for now (easier to debug)
    // ESP_LOGD(LOG_TAG, "SPI payload sent!");
}

void lvgl_st7735r_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color)
{
    uint32_t line_size = area->x2 - area->x1 + 1;
    st7735r_set_addr_window(area->x1, area->x2, area->y1, area->y2);
    st7735r_spi_send_cmd(ST7735_RAMWR);

    for(uint32_t curr_y = area->y1; curr_y <= area->y2; curr_y++) {
        st7735r_spi_send_pixel(&color->full, line_size);
        color += line_size;
    }

    lv_disp_flush_ready(disp_drv);
}

void lvgl_st7735r_init()
{
    ESP_LOGI(LOG_TAG, "Performing GPIO init...");
    ESP_ERROR_CHECK(gpio_set_direction(CONFIG_LVGL_IO_DC, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(CONFIG_LVGL_IO_RST, GPIO_MODE_OUTPUT));
    ESP_LOGI(LOG_TAG, "GPIO initialization finished, resetting OLED...");

    ESP_ERROR_CHECK(gpio_set_level(CONFIG_LVGL_IO_RST, 0));
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_ERROR_CHECK(gpio_set_level(CONFIG_LVGL_IO_RST, 1));
    vTaskDelay(pdMS_TO_TICKS(150));

    spi_bus_config_t bus_config = {
            .mosi_io_num = CONFIG_LVGL_SPI_MOSI,
            .sclk_io_num = CONFIG_LVGL_SPI_SCLK,
            .miso_io_num = -1, // We don't need to get any bullshit from the panel, so no MISO needed.
            .quadhd_io_num = -1,
            .quadwp_io_num = -1,
            .max_transfer_sz = ST7735R_BUF_SIZE * 2
    };

    spi_device_interface_config_t device_config = {
#ifndef CONFIG_LVGL_SPI_CLK_DEBUG
            .clock_speed_hz = SPI_MASTER_FREQ_26M,
#else
            .clock_speed_hz = SPI_MASTER_FREQ_8M,
#endif
            .mode = 0, // CPOL = 0, CPHA = 0???
            .spics_io_num = CONFIG_LVGL_SPI_CS,
            .queue_size = 7
    };

    ESP_LOGI(LOG_TAG, "Performing SPI init...");
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &bus_config, 1));
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &device_config, &device_handle));
    ESP_LOGI(LOG_TAG, "SPI initialization finished, sending init sequence to IPS panel...");

    // Wake up
    st7735r_spi_send_cmd(ST7735_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(200));

    // Send off the init sequence
    for(size_t idx = 0; idx < (sizeof(st7735s_init_seq) / sizeof(st7735s_init_seq[0])); idx++) {
        st7735r_send_init_seq(&st7735s_init_seq[idx]);
    }

    // Normal display ON
    st7735r_spi_send_cmd(ST7735_NORON);
    vTaskDelay(pdMS_TO_TICKS(10));

    st7735r_spi_send_cmd(ST7735_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));

    // Fill screen
    st7735r_set_addr_window(0, 0x4f, 0, 0x9f);
    st7735r_spi_send_cmd(ST7735_RAMWR);
    const uint16_t white = 0x0000;
    for(uint32_t idx = 0; idx < (80 * 160 * 2); idx++) {
        st7735r_spi_send_pixel(&white, 1);
    }
}
/******************************************************************************
 * @file    environment_x6.c
 * @brief   iNose X6 gas sensor driver (ESP-IDF)
 ******************************************************************************/

#include "environment_x6.h"

#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "ENV_X6";


/* ---------------- checksum ---------------- */

static uint8_t checksum_0_add8(uint8_t *buf, uint8_t len)
{
    uint8_t sum = 0;

    for(int i = 0; i < len; i++)
        sum += buf[i];

    return (uint8_t)(0 - sum);
}


/* ---------------- float convert ---------------- */

static float bytes_to_float(uint8_t *buf)
{
    uint32_t temp =
        (buf[0] << 24) |
        (buf[1] << 16) |
        (buf[2] << 8)  |
        buf[3];

    float value;
    memcpy(&value, &temp, 4);

    return value;
}


/* ---------------- debug print ---------------- */

static void print_rx(uint8_t *buf, int len)
{
    printf("RX: ");

    for(int i = 0; i < len; i++)
        printf("%02X ", buf[i]);

    printf("\n");
}


/* ---------------- uart read ---------------- */

static bool read_bytes(uint8_t *buf, int len)
{
    int ret = uart_read_bytes(
        ENV_X6_UART_NUM,
        buf,
        len,
        pdMS_TO_TICKS(500));

    if(ret != len)
        return false;

    // print_rx(buf, len);

    return true;
}


/* ---------------- init ---------------- */

void environment_x6_init(void)
{
    uart_config_t uart_config =
    {
        .baud_rate = ENV_X6_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_driver_install(ENV_X6_UART_NUM, 2048, 0, 0, NULL, 0);

    uart_param_config(ENV_X6_UART_NUM, &uart_config);

    uart_set_pin(
        ENV_X6_UART_NUM,
        ENV_X6_UART_TX,
        ENV_X6_UART_RX,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE);

    ESP_LOGI(TAG, "X6 Sensor Init Done");
}


/* ---------------- concentration ---------------- */

bool environment_x6_get_concentration(environment_x6_data_t *data)
{
    uint8_t cmd[2] = {ENV_X6_CMD_GET_CONC, 0};
    cmd[1] = checksum_0_add8(cmd, 1);

    uint8_t rx[22];

    uart_write_bytes(ENV_X6_UART_NUM, (char*)cmd, 2);

    if(!read_bytes(rx, 22))
        return false;

    if(checksum_0_add8(rx,21) != rx[21])
        return false;

    data->iaq  = bytes_to_float(&rx[1]);
    data->tvoc = bytes_to_float(&rx[5]);
    data->hcho = bytes_to_float(&rx[9]);
    data->co   = bytes_to_float(&rx[13]);

    uint16_t t = (rx[17]<<8) | rx[18];
    uint16_t h = (rx[19]<<8) | rx[20];

    data->temperature = t / 100.0f;
    data->humidity    = h / 100.0f;

    return true;
}


/* ---------------- sensor param ---------------- */

bool environment_x6_get_sensor_param(uint8_t index,
                                     environment_x6_sensor_param_t *param)
{
    uint8_t cmd[3] = {ENV_X6_CMD_GET_SENSOR, index, 0};
    cmd[2] = checksum_0_add8(cmd, 2);

    uint8_t rx[7];

    uart_write_bytes(ENV_X6_UART_NUM, (char*)cmd, 3);

    if(!read_bytes(rx,7))
        return false;

    if(checksum_0_add8(rx,6) != rx[6])
        return false;

    param->type  = rx[2];
    param->range = (rx[3]<<8) | rx[4];
    param->unit  = rx[5];

    return true;
}


/* ---------------- gas name ---------------- */

const char* environment_x6_get_gas_name(uint8_t type)
{
    switch(type)
    {
        case GAS_HCHO: return "HCHO";
        case GAS_TVOC: return "TVOC";
        case GAS_CO:   return "CO";
        case GAS_IAQ:  return "IAQ";
        default:       return "Unknown";
    }
}


/* ---------------- LED ---------------- */

bool environment_x6_get_led(uint8_t *status)
{
    uint8_t cmd[2] = {ENV_X6_CMD_GET_LED,0};
    cmd[1] = checksum_0_add8(cmd,1);

    uint8_t rx[3];

    uart_write_bytes(ENV_X6_UART_NUM,(char*)cmd,2);

    if(!read_bytes(rx,3))
        return false;

    if(checksum_0_add8(rx,2) != rx[2])
        return false;

    *status = rx[1];

    return true;
}


bool environment_x6_set_led(uint8_t status)
{
    uint8_t cmd[3] = {ENV_X6_CMD_SET_LED,status,0};
    cmd[2] = checksum_0_add8(cmd,2);

    uint8_t rx[5];

    uart_write_bytes(ENV_X6_UART_NUM,(char*)cmd,3);

    if(!read_bytes(rx,5))
        return false;

    return true;
}


/* ---------------- sleep ---------------- */

bool environment_x6_sleep(void)
{
    uint8_t cmd[7] =
    {ENV_X6_CMD_SLEEP,0x73,0x6C,0x65,0x65,0x70,0};

    cmd[6] = checksum_0_add8(cmd,6);

    uart_write_bytes(ENV_X6_UART_NUM,(char*)cmd,7);

    uint8_t rx[5];

    return read_bytes(rx,5);
}


/* ---------------- wakeup ---------------- */

bool environment_x6_wakeup(void)
{
    uint8_t cmd[7] =
    {ENV_X6_CMD_WAKEUP,0x61,0x77,0x61,0x6B,0x65,0};

    cmd[6] = checksum_0_add8(cmd,6);

    uart_write_bytes(ENV_X6_UART_NUM,(char*)cmd,7);

    uint8_t rx[5];

    return read_bytes(rx,5);
}


/* ---------------- serial ---------------- */

bool environment_x6_get_serial(char *sn)
{
    uint8_t cmd[2] = {ENV_X6_CMD_GET_SN,0};
    cmd[1] = checksum_0_add8(cmd,1);

    uint8_t rx[8];

    uart_write_bytes(ENV_X6_UART_NUM,(char*)cmd,2);

    if(!read_bytes(rx,8))
        return false;

    if(checksum_0_add8(rx,7) != rx[7])
        return false;

    sprintf(sn,"%02X%02X%02X%02X%02X%02X",
        rx[1],rx[2],rx[3],rx[4],rx[5],rx[6]);

    return true;
}


/* ---------------- version ---------------- */

bool environment_x6_get_version(char *ver)
{
    uint8_t cmd[2] = {ENV_X6_CMD_GET_VER,0};
    cmd[1] = checksum_0_add8(cmd,1);

    uint8_t rx[21];

    uart_write_bytes(ENV_X6_UART_NUM,(char*)cmd,2);

    if(!read_bytes(rx,21))
        return false;

    if(checksum_0_add8(rx,20) != rx[20])
        return false;

    memcpy(ver,&rx[1],19);
    ver[19] = 0;

    return true;
}
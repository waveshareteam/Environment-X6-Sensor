/******************************************************************************
 * @file    environment_x6.c
 * @brief   iNose X6 gas sensor module UART driver implementation
 * @author  Waveshare
 * @license BSD-3-Clause / Waveshare Open Source
 ******************************************************************************/

#include "environment_x6.h"
#include <string.h>

/* ----------------- 0-ADD8 CHECKSUM ----------------- */
static uint8_t checksum_0_add8(uint8_t *buf, uint8_t len)
{
    uint8_t sum = 0;
    for(uint8_t i = 0; i < len; i++)
        sum += buf[i];
    return (uint8_t)(0 - sum);
}

/* ----------------- CONVERT 4 BYTES TO FLOAT ----------------- */
static float bytes_to_float(uint8_t *buf)
{
    uint32_t temp = ((uint32_t)buf[0] << 24) |
                    ((uint32_t)buf[1] << 16) |
                    ((uint32_t)buf[2] << 8)  |
                    ((uint32_t)buf[3]);
    float value;
    memcpy(&value, &temp, 4);
    return value;
}

/* ----------------- READ SPECIFIC NUMBER OF BYTES ----------------- */
static bool read_bytes(uint8_t *buf, uint16_t len, uint32_t timeout_ms)
{
    uint32_t start = millis();
    uint16_t idx = 0;
    while(idx < len)
    {
        if(ENVIRONMENTX6_SERIAL.available())
            buf[idx++] = ENVIRONMENTX6_SERIAL.read();
        if(millis() - start > timeout_ms)
            return false;
    }
    return true;
}

/* ----------------- INITIALIZE UART ----------------- */
void environment_x6_init(void)
{
    Serial.begin(115200);
    ENVIRONMENTX6_SERIAL.setPins(ENVIRONMENTX6_RX, ENVIRONMENTX6_TX);
    ENVIRONMENTX6_SERIAL.begin(9600);
}

static void print_bytes(uint8_t *buf, uint16_t len)
{
    Serial.print("RX: ");
    for(uint16_t i = 0; i < len; i++)
    {
        if(buf[i] < 0x10) Serial.print("0"); // padding
        Serial.print(buf[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

/* ----------------- READ GAS CONCENTRATION ----------------- */
bool environment_x6_get_concentration(EnvironmentX6_Data *data)
{
    uint8_t cmd[2] = {ENV_X6_CMD_GET_CONC, 0};
    cmd[1] = checksum_0_add8(cmd, 1);

    uint8_t rx[22];
    ENVIRONMENTX6_SERIAL.write(cmd, 2);

    if(!read_bytes(rx, 22, 500)) return false;
    if(checksum_0_add8(rx, 21) != rx[21]) return false;
    // print_bytes(rx, 22);

    data->iaq  = bytes_to_float(&rx[1]);
    data->tvoc = bytes_to_float(&rx[5]);
    data->hcho = bytes_to_float(&rx[9]);
    data->co   = bytes_to_float(&rx[13]);

    uint16_t t = (rx[17] << 8) | rx[18];
    uint16_t h = (rx[19] << 8) | rx[20];

    data->temperature = t / 100.0f;
    data->humidity    = h / 100.0f;

    return true;
}

/* ----------------- READ SENSOR PARAMETERS ----------------- */
bool environment_x6_get_sensor_param(uint8_t index, EnvironmentX6_SensorParam *param)
{
    uint8_t cmd[3] = {ENV_X6_CMD_GET_SENSOR, index, 0};
    cmd[2] = checksum_0_add8(cmd, 2);

    uint8_t rx[7];
    ENVIRONMENTX6_SERIAL.write(cmd, 3);

    if(!read_bytes(rx, 7, 500)) return false;
    if(checksum_0_add8(rx, 6) != rx[6]) return false;

    param->type  = rx[2];
    param->range = (rx[3] << 8) | rx[4];
    param->unit  = rx[5];

    return true;
}

/* ----------------- GET GAS NAME BY TYPE ----------------- */
const char* get_gas_name_by_type(uint8_t type)
{
    switch(type)
    {
        case HCHO: return "HCHO";
        case TVOC: return "TVOC";
        case CO:   return "CO";
        case IAQ:  return "IAQ";
        default:   return "Unknown";
    }
}

/* ----------------- LED FUNCTIONS ----------------- */
bool environment_x6_get_led_status(uint8_t *status)
{
    uint8_t cmd[2] = {ENV_X6_CMD_GET_LED, 0};
    cmd[1] = checksum_0_add8(cmd, 1);

    uint8_t rx[3];
    ENVIRONMENTX6_SERIAL.write(cmd, 2);

    if(!read_bytes(rx, 3, 500)) return false;
    if(checksum_0_add8(rx, 2) != rx[2]) return false;

    *status = rx[1];
    return true;
}

bool environment_x6_set_led_status(uint8_t status)
{
    uint8_t cmd[3] = {ENV_X6_CMD_SET_LED, status, 0};
    cmd[2] = checksum_0_add8(cmd, 2);

    uint8_t rx[5];
    ENVIRONMENTX6_SERIAL.write(cmd, 3);

    if(!read_bytes(rx, 5, 500)) return false;
    if(checksum_0_add8(rx, 4) != rx[4]) return false;

    return true;
}

/* ----------------- SLEEP / WAKEUP ----------------- */
bool environment_x6_sleep(void)
{
    uint8_t cmd[7] = {ENV_X6_CMD_SLEEP, 0x73, 0x6C, 0x65, 0x65, 0x70, 0};
    cmd[6] = checksum_0_add8(cmd, 6);

    uint8_t rx[5];
    ENVIRONMENTX6_SERIAL.write(cmd, 7);

    if(!read_bytes(rx, 5, 500)) return false;
    return true;
}

bool environment_x6_wakeup(void)
{
    uint8_t cmd[7] = {ENV_X6_CMD_WAKEUP, 0x61, 0x77, 0x61, 0x6B, 0x65, 0};
    cmd[6] = checksum_0_add8(cmd, 6);

    uint8_t rx[5];
    ENVIRONMENTX6_SERIAL.write(cmd, 7);

    if(!read_bytes(rx, 5, 500)) return false;
    return true;
}

/* ----------------- GET SERIAL NUMBER ----------------- */
bool environment_x6_get_serial(char *sn)
{
    uint8_t cmd[2] = {ENV_X6_CMD_GET_SERIAL, 0};
    cmd[1] = checksum_0_add8(cmd, 1);

    uint8_t rx[8];
    ENVIRONMENTX6_SERIAL.write(cmd, 2);

    if(!read_bytes(rx, 8, 500)) return false;
    if(checksum_0_add8(rx, 7) != rx[7]) return false;

    sprintf(sn, "%02X%02X%02X%02X%02X%02X",
            rx[1], rx[2], rx[3], rx[4], rx[5], rx[6]);
    return true;
}

/* ----------------- GET SOFTWARE VERSION ----------------- */
bool environment_x6_get_version(char *ver)
{
    uint8_t cmd[2] = {ENV_X6_CMD_GET_VERSION, 0};
    cmd[1] = checksum_0_add8(cmd, 1);

    uint8_t rx[21];
    ENVIRONMENTX6_SERIAL.write(cmd, 2);

    if(!read_bytes(rx, 21, 500)) return false;
    if(checksum_0_add8(rx, 20) != rx[20]) return false;

    memcpy(ver, &rx[1], 19);
    ver[19] = '\0';
    return true;
}
/******************************************************************************
 * @file    environment_x6.h
 * @brief   iNose X6 gas sensor UART driver for ESP-IDF
 * @author  Waveshare
 * @license BSD-3-Clause
 ******************************************************************************/

#ifndef __ENVIRONMENT_X6_H
#define __ENVIRONMENT_X6_H

#include <stdint.h>
#include <stdbool.h>

/* UART CONFIG */
#define ENV_X6_UART_NUM UART_NUM_2
#define ENV_X6_UART_TX  38
#define ENV_X6_UART_RX  39
#define ENV_X6_BAUDRATE 9600

/* COMMANDS */
#define ENV_X6_CMD_GET_CONC    0x70
#define ENV_X6_CMD_GET_SENSOR  0x72
#define ENV_X6_CMD_GET_LED     0x74
#define ENV_X6_CMD_SET_LED     0x56
#define ENV_X6_CMD_SLEEP       0x54
#define ENV_X6_CMD_WAKEUP      0x55
#define ENV_X6_CMD_GET_SN      0x71
#define ENV_X6_CMD_GET_VER     0x73

/* GAS TYPE */
#define GAS_HCHO 0x17
#define GAS_TVOC 0x18
#define GAS_CO   0x19
#define GAS_IAQ  0x33

typedef struct
{
    float hcho;
    float tvoc;
    float co;
    float iaq;
    float temperature;
    float humidity;

} environment_x6_data_t;

typedef struct
{
    uint8_t type;
    uint16_t range;
    uint8_t unit;

} environment_x6_sensor_param_t;


/* API */

void environment_x6_init(void);

bool environment_x6_get_concentration(environment_x6_data_t *data);

bool environment_x6_get_sensor_param(uint8_t index,
                                     environment_x6_sensor_param_t *param);

bool environment_x6_get_led(uint8_t *status);

bool environment_x6_set_led(uint8_t status);

bool environment_x6_sleep(void);

bool environment_x6_wakeup(void);

bool environment_x6_get_serial(char *sn);

bool environment_x6_get_version(char *ver);

const char* environment_x6_get_gas_name(uint8_t type);

#endif
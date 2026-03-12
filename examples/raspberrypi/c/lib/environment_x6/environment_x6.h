#ifndef __ENVIRONMENT_X6_H
#define __ENVIRONMENT_X6_H

#include <stdint.h>
#include <stdbool.h>
#include "DEV_Config.h"

/* ----------------- X6 commands ----------------- */
#define ENV_X6_CMD_GET_CONC        0x70
#define ENV_X6_CMD_GET_SENSOR      0x72
#define ENV_X6_CMD_GET_LED         0x74
#define ENV_X6_CMD_SET_LED         0x56
#define ENV_X6_CMD_SLEEP           0x54
#define ENV_X6_CMD_WAKEUP          0x55
#define ENV_X6_CMD_GET_SERIAL      0x71
#define ENV_X6_CMD_GET_VERSION     0x73

#define HCHO 0x17
#define TVOC 0x18
#define CO   0x19
#define IAQ  0x33

/* ----------------- Data structures ----------------- */
typedef struct {
    float hcho;
    float tvoc;
    float co;
    float iaq;
    float temperature;
    float humidity;
} environment_x6_data_t;

typedef struct {
    uint8_t type;
    uint16_t range;
    uint8_t unit;
} environment_x6_sensor_param_t;

/* ----------------- API ----------------- */
void environment_x6_init(uint32_t baud);

bool environment_x6_get_concentration(environment_x6_data_t *data);
bool environment_x6_get_sensor_param(uint8_t index, environment_x6_sensor_param_t *param);
const char* get_gas_name_by_type(uint8_t type);
bool environment_x6_get_led_status(uint8_t *status);
bool environment_x6_set_led_status(uint8_t status);
bool environment_x6_sleep(void);
bool environment_x6_wakeup(void);
bool environment_x6_get_serial(char *sn);
bool environment_x6_get_version(char *ver);

#endif
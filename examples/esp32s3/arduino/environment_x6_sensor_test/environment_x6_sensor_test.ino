/******************************************************************************
 * @file    environment_x6_sensor_test.ino
 * @brief   Arduino test example for iNose X6 gas sensor module
 * @author  Waveshare
 * @license BSD-3-Clause / Waveshare Open Source
 ******************************************************************************/

#include "environment_x6.h"

/* ----------------- TEST CONFIG ----------------- */
#define TEST_CONCENTRATION    1
#define TEST_SENSOR_PARAM     0
#define TEST_LED              0
#define TEST_SLEEP_WAKEUP     0
#define TEST_SERIAL           0
#define TEST_VERSION          0

/* ----------------- GLOBAL VARIABLES ----------------- */
EnvironmentX6_Data env;
EnvironmentX6_SensorParam param;
uint8_t led_status;
char serial_number[20];
char version[20];

/* ----------------- SETUP ----------------- */
void setup() {
    environment_x6_init();
    uint8_t cmd[2] = {0x53, 0xad};
    ENVIRONMENTX6_SERIAL.write(cmd, 2);
    Serial.println("=== iNose X6 Test Start ===");
}

/* ----------------- MAIN LOOP ----------------- */
void loop() 
{
#if TEST_CONCENTRATION
    if(environment_x6_get_concentration(&env))
    {
        Serial.println("--- Concentration ---");
        Serial.print("HCHO: "); Serial.println(env.hcho, 3);
        Serial.print("TVOC: "); Serial.println(env.tvoc, 3);
        Serial.print("CO: "); Serial.println(env.co, 3);
        Serial.print("IAQ: "); Serial.println(env.iaq, 3);
        Serial.print("Temperature: "); Serial.println(env.temperature, 2);
        Serial.print("Humidity: "); Serial.println(env.humidity, 2);
    }
    else
    {
        Serial.println("Failed to read concentration");
    }
#endif

#if TEST_SENSOR_PARAM
    /* Loop through all gas types */
    uint8_t gas_indices[] = {IAQ, TVOC, HCHO, CO };
    for(uint8_t i = 0; i < 4; i++)
    {
        if(environment_x6_get_sensor_param(i, &param))
        {
            Serial.print("--- Sensor Param (");
            Serial.print(get_gas_name_by_type(param.type));
            Serial.println(") ---");

            Serial.print("Type: 0x"); 
            if(param.type < 0x10) Serial.print("0"); // padding for single-digit HEX
            Serial.println(param.type, HEX);

            Serial.print("Range: "); Serial.println(param.range);
            Serial.print("Unit: "); Serial.println(param.unit);
        }
        else
        {
            Serial.print("Failed to read sensor param for "); 
            Serial.println(get_gas_name_by_type(gas_indices[i]));
        }
    }
#endif

#if TEST_LED
    /* Read LED status */
    if(environment_x6_get_led_status(&led_status))
    {
        Serial.print("LED Status: "); 
        Serial.println(led_status ? "ON" : "OFF");
    }
    else
    {
        Serial.println("Failed to read LED status");
    }

    /* Toggle LED */
    environment_x6_set_led_status(led_status ? 0 : 1);
#endif

#if TEST_SLEEP_WAKEUP
    /* Sleep/Wake test */
    environment_x6_sleep();
    delay(1000);
    environment_x6_wakeup();
#endif

#if TEST_SERIAL
    if(environment_x6_get_serial(serial_number))
    {
        Serial.print("Serial Number: "); Serial.println(serial_number);
    }
    else
    {
        Serial.println("Failed to get serial number");
    }
#endif

#if TEST_VERSION
    if(environment_x6_get_version(version))
    {
        Serial.print("Software Version: "); Serial.println(version);
    }
    else
    {
        Serial.println("Failed to get software version");
    }
#endif

    Serial.println("=== Loop End ===\n");
    delay(2000);
}
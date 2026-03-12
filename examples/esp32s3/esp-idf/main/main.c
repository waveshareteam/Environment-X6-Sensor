#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "environment_x6.h"

#define TEST_CONCENTRATION    1
#define TEST_SENSOR_PARAM     0
#define TEST_LED              0
#define TEST_SLEEP_WAKEUP     0
#define TEST_SERIAL           0
#define TEST_VERSION          0

void app_main()
{
    environment_x6_init();

    while(1)
    {

#if TEST_CONCENTRATION
        environment_x6_data_t env;
        if(environment_x6_get_concentration(&env))
        {
            printf("--- Concentration ---\n");
            printf("HCHO: %.3f\n", env.hcho);
            printf("TVOC: %.3f\n", env.tvoc);
            printf("CO: %.3f\n", env.co);
            printf("IAQ: %.3f\n", env.iaq);
            printf("Temp: %.2f\n", env.temperature);
            printf("Hum: %.2f\n", env.humidity);
        }
        else
        {
            printf("Failed to read concentration\n");
        }

#endif


#if TEST_SENSOR_PARAM
        environment_x6_sensor_param_t param;
        for(int i = 0; i < 4; i++)
        {
            if(environment_x6_get_sensor_param(i, &param))
            {
                printf("--- Sensor Param ---\n");
                printf("Type: 0x%02X\n", param.type);
                printf("Range: %d\n", param.range);
                printf("Unit: %d\n", param.unit);
            }
        }

#endif


#if TEST_LED
        uint8_t led_status;
        if(environment_x6_get_led(&led_status))
        {
            printf("LED Status: %s\n", led_status ? "ON" : "OFF");
        }
        else
        {
            printf("Failed to read LED status\n");
        }

        environment_x6_set_led(!led_status);

#endif


#if TEST_SLEEP_WAKEUP

        printf("Entering sleep...\n");
        environment_x6_sleep();

        vTaskDelay(pdMS_TO_TICKS(2000));

        printf("Waking up...\n");
        environment_x6_wakeup();

#endif


#if TEST_SERIAL
        char sn[32];
        if(environment_x6_get_serial(sn))
        {
            printf("Serial Number: %s\n", sn);
        }
        else
        {
            printf("Failed to get serial number\n");
        }

#endif


#if TEST_VERSION
        char ver[32];
        if(environment_x6_get_version(ver))
        {
            printf("Version: %s\n", ver);
        }
        else
        {
            printf("Failed to get version\n");
        }

#endif


        printf("=== Loop End ===\n\n");

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
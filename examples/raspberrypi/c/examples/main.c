#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "environment_x6.h"

/* ----------------- Select which tests to run ----------------- */
#define TEST_CONCENTRATION    1
#define TEST_SENSOR_PARAM     0
#define TEST_LED              0
#define TEST_SLEEP_WAKEUP     0
#define TEST_SERIAL           0
#define TEST_VERSION          0

int main(void)
{
    uint32_t baud = 9600;

    printf("=== iNose X6 Test Start ===\n");

    environment_x6_init(baud);

    UART_flushInput();       // 清空 UART 缓冲
    DEV_Delay_ms(100);        // 延时 100ms，确保模块准备好

    while (1)
    {
#if TEST_CONCENTRATION
        {
            environment_x6_data_t env;
            if(environment_x6_get_concentration(&env))
            {
                printf("--- Concentration ---\n");
                printf("HCHO: %.3f ppm\n", env.hcho);
                printf("TVOC: %.3f ppm\n", env.tvoc);
                printf("CO: %.3f ppm\n", env.co);
                printf("IAQ: %.3f\n", env.iaq);
                printf("Temperature: %.2f C\n", env.temperature);
                printf("Humidity: %.2f %%\n", env.humidity);
            }
            else
            {
                printf("Failed to read concentration\n");
            }
        }
#endif

#if TEST_SENSOR_PARAM
        {
            environment_x6_sensor_param_t param;
            for(uint8_t i = 0; i < 4; i++)
            {
                if(environment_x6_get_sensor_param(i, &param))
                {
                    printf("--- Sensor Param (%s) ---\n", get_gas_name_by_type(param.type));
                    printf("Type: 0x%02X\n", param.type);
                    printf("Range: %d\n", param.range);
                    printf("Unit: %d\n", param.unit);
                }
                else
                {
                    printf("Failed to read sensor param for index %d\n", i);
                }
            }
        }
#endif

#if TEST_LED
        {
            uint8_t led;
            if(environment_x6_get_led_status(&led))
            {
                printf("LED Status: %s\n", led ? "ON" : "OFF");
                // Toggle LED
                environment_x6_set_led_status(led ? 1 : 1);
                printf("LED toggled.\n");
            }
            else
            {
                printf("Failed to read LED status\n");
            }
        }
#endif

#if TEST_SLEEP_WAKEUP
        {
            printf("Entering sleep mode...\n");
            environment_x6_sleep();
            DEV_Delay_ms(1000); // 1 second
            printf("Waking up...\n");
            environment_x6_wakeup();
            DEV_Delay_ms(1000); // 1 second 确保已经唤醒
            
        }
#endif

#if TEST_SERIAL
        {
            char sn[32];
            if(environment_x6_get_serial(sn))
            {
                printf("Serial Number: %s\n", sn);
            }
            else
            {
                printf("Failed to get serial number\n");
            }
        }
#endif

#if TEST_VERSION
        {
            char ver[32];
            if(environment_x6_get_version(ver))
            {
                printf("Version: %s\n", ver);
            }
            else
            {
                printf("Failed to get version\n");
            }
        }
#endif

    printf("=== Test End ===\n\n");
    DEV_Delay_ms(1000);
}


    return 0;
}
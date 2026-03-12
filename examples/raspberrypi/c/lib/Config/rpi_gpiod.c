#include "rpi_gpiod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gpiod.h>

struct gpiod_chip *gpiochip = NULL;

// 每个 Pin 保存 gpiod_line_request* 和 line offset
struct {
    struct gpiod_line_request *request;
    unsigned int offset;
} line_map[64] = {0};

int GPIOD_Export()
{
    char buffer[NUM_MAXBUF];
    FILE *fp;

    fp = popen("cat /proc/cpuinfo | grep 'Raspberry Pi 5'", "r");
    if (!fp) {
        GPIOD_Debug("Cannot detect Raspberry Pi model\n");
        return -1;
    }

    if(fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        gpiochip = gpiod_chip_open("/dev/gpiochip4");
        if (!gpiochip) {
            GPIOD_Debug("Failed to open gpiochip4\n");
            return -1;
        }
    }
    else
    {
        gpiochip = gpiod_chip_open("/dev/gpiochip0");
        if (!gpiochip) {
            GPIOD_Debug("Failed to open gpiochip0\n");
            return -1;
        }
    }
    return 0;
}

int GPIOD_Unexport(int Pin)
{
    if (Pin < 0 || Pin >= 64) return -1;
    if (line_map[Pin].request) {
        gpiod_line_request_release(line_map[Pin].request);
        line_map[Pin].request = NULL;
    }
    GPIOD_Debug("Unexport Pin %d\n", Pin);
    return 0;
}

int GPIOD_Unexport_GPIO(void)
{
    for (int i = 0; i < 64; i++) {
        if (line_map[i].request) {
            gpiod_line_request_release(line_map[i].request);
            line_map[i].request = NULL;
        }
    }
    if (gpiochip) {
        gpiod_chip_close(gpiochip);
        gpiochip = NULL;
    }
    return 0;
}

int GPIOD_Direction(int Pin, int Dir)
{
    if (!gpiochip || Pin < 0 || Pin >= 64) return -1;

    struct gpiod_line_settings *settings = gpiod_line_settings_new();
    if (!settings) return -1;

    gpiod_line_settings_set_direction(settings,
        Dir == GPIOD_IN ? GPIOD_LINE_DIRECTION_INPUT : GPIOD_LINE_DIRECTION_OUTPUT);

    struct gpiod_line_config *line_cfg = gpiod_line_config_new();
    if (!line_cfg) return -1;

    gpiod_line_config_add_line_settings(line_cfg, (unsigned int*)&Pin, 1, settings);

    struct gpiod_request_config *req_cfg = gpiod_request_config_new();
    if (!req_cfg) return -1;

    gpiod_request_config_set_consumer(req_cfg, "gpio_user");

    struct gpiod_line_request *request = gpiod_chip_request_lines(gpiochip, req_cfg, line_cfg);
    if (!request) {
        GPIOD_Debug("Failed to request GPIO %d\n", Pin);
        return -1;
    }

    line_map[Pin].request = request;
    line_map[Pin].offset = Pin;

    GPIOD_Debug("Pin %d %s\n", Pin, (Dir==GPIOD_IN)?"INPUT":"OUTPUT");
    return 0;
}

int GPIOD_Read(int Pin)
{
    if (Pin < 0 || Pin >= 64 || !line_map[Pin].request) return -1;

    int value = gpiod_line_request_get_value(line_map[Pin].request, line_map[Pin].offset);
    if (value < 0) {
        GPIOD_Debug("Failed to read Pin %d\n", Pin);
        return -1;
    }
    return value;
}

int GPIOD_Write(int Pin, int value)
{
    if (Pin < 0 || Pin >= 64 || !line_map[Pin].request) return -1;

    int ret = gpiod_line_request_set_value(line_map[Pin].request, line_map[Pin].offset, value);
    if (ret < 0) {
        GPIOD_Debug("Failed to write Pin %d\n", Pin);
        return -1;
    }
    return 0;
}
/*****************************************************************************
* | File        :   dev_hardware_UART.c
* | Author      :   Waveshare team
* | Function    :   Read and write /dev/UART,  hardware UART
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-06-26
* | Info        :   Basic version
*
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "dev_hardware_UART.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

HARDWARE_UART hardware_UART;
struct termios UART_Set;

/* ---------------- UART init ---------------- */
void DEV_HARDWARE_UART_begin(char *UART_device)
{
    if ((hardware_UART.fd = open(UART_device, O_RDWR | O_NOCTTY)) < 0)
    {
        perror("Failed to open UART device");
        exit(1);
    }

    DEV_HARDWARE_UART_setBaudrate(9600);
    DEV_HARDWARE_UART_Set(8,1,'N');

    // flush input buffer
    tcflush(hardware_UART.fd, TCIFLUSH);
}

/* ---------------- Set baudrate ---------------- */
void DEV_HARDWARE_UART_setBaudrate(uint32_t Baudrate)
{
    uint32_t baud;
    switch (Baudrate)
    {
        case 921600: baud = B921600; break;
        case 460800: baud = B460800; break;
        case 230400: baud = B230400; break;
        case 115200: baud = B115200; break;
        case 57600:  baud = B57600;  break;
        case 38400:  baud = B38400;  break;
        case 19200:  baud = B19200;  break;
        case 9600:   baud = B9600;   break;
        default:     baud = B9600;   break;
    }
    cfsetispeed(&UART_Set, baud);
    cfsetospeed(&UART_Set, baud);
    tcsetattr(hardware_UART.fd, TCSANOW, &UART_Set);
}

/* ---------------- UART settings ---------------- */
int DEV_HARDWARE_UART_Set(int databits, int stopbits, int parity)
{
    if(tcgetattr(hardware_UART.fd, &UART_Set) != 0)
        return 0;

    UART_Set.c_cflag |= (CLOCAL | CREAD);

    switch(databits)
    {
        case 5: UART_Set.c_cflag = (UART_Set.c_cflag & ~CSIZE) | CS5; break;
        case 6: UART_Set.c_cflag = (UART_Set.c_cflag & ~CSIZE) | CS6; break;
        case 7: UART_Set.c_cflag = (UART_Set.c_cflag & ~CSIZE) | CS7; break;
        case 8: UART_Set.c_cflag = (UART_Set.c_cflag & ~CSIZE) | CS8; break;
        default: return 0;
    }

    switch(parity)
    {
        case 'N': case 'n': UART_Set.c_cflag &= ~PARENB; break;
        case 'O': case 'o': UART_Set.c_cflag |= PARENB | PARODD; break;
        case 'E': case 'e': UART_Set.c_cflag |= PARENB; UART_Set.c_cflag &= ~PARODD; break;
        default: return 0;
    }

    switch(stopbits)
    {
        case 1: UART_Set.c_cflag &= ~CSTOPB; break;
        case 2: UART_Set.c_cflag |= CSTOPB; break;
        default: return 0;
    }

    UART_Set.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    UART_Set.c_oflag &= ~OPOST;
    UART_Set.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL | INLCR | ISTRIP);

    // 阻塞模式：至少读 1 字节
    UART_Set.c_cc[VMIN]  = 1;
    UART_Set.c_cc[VTIME] = 10; // 1s timeout (VTIME*0.1s)

    tcflush(hardware_UART.fd, TCIFLUSH);
    if(tcsetattr(hardware_UART.fd, TCSANOW, &UART_Set) != 0)
        return 0;

    return 1;
}

/* ---------------- Write bytes ---------------- */
uint8_t DEV_HARDWARE_UART_writeByte(uint8_t buf)
{
    write(hardware_UART.fd, &buf, 1);
    return 0;
}

uint8_t DEV_HARDWARE_UART_write(uint8_t *buf, uint32_t len)
{
    write(hardware_UART.fd, buf, len);
    return 0;
}

/* ---------------- Read bytes ---------------- */
uint8_t DEV_HARDWARE_UART_readByte(void)
{
    uint8_t buf;
    read(hardware_UART.fd, &buf, 1);
    return buf;
}

uint32_t DEV_HARDWARE_UART_read(uint8_t *buf, uint32_t len)
{
    uint32_t total = 0;
    while(total < len)
    {
        int r = read(hardware_UART.fd, buf+total, len-total);
        if(r > 0) total += r;
        else usleep(1000); // 等待 1ms
    }
    return total;
}

/* ---------------- Flush input ---------------- */
void DEV_HARDWARE_UART_flushInput(void)
{
    tcflush(hardware_UART.fd, TCIFLUSH);
}
import serial
import time
import struct

# ----------------- Commands -----------------
ENV_X6_CMD_GET_CONC    = 0x70
ENV_X6_CMD_GET_SENSOR  = 0x72
ENV_X6_CMD_GET_LED     = 0x74
ENV_X6_CMD_SET_LED     = 0x56
ENV_X6_CMD_SLEEP       = 0x54
ENV_X6_CMD_WAKEUP      = 0x55
ENV_X6_CMD_GET_SERIAL  = 0x71
ENV_X6_CMD_GET_VERSION = 0x73

# ----------------- Gas Types -----------------
HCHO = 0x17
TVOC = 0x18
CO   = 0x19
IAQ  = 0x33

GAS_NAME = {HCHO:"HCHO", TVOC:"TVOC", CO:"CO", IAQ:"IAQ"}

# ----------------- X6 Driver -----------------
class EnvironmentX6:
    def __init__(self, port="/dev/ttyS0", baudrate=9600, timeout=0.1):
        self.ser = serial.Serial(port, baudrate=baudrate, timeout=timeout)
        time.sleep(0.1)  # wait UART stable

    # ---------------- checksum ----------------
    @staticmethod
    def checksum_0_add8(buf):
        s = sum(buf) & 0xFF
        return (-s) & 0xFF

    # ---------------- float helper ----------------
    @staticmethod
    def bytes_to_float(b):
        return struct.unpack('>f', bytes(b))[0]

    # ---------------- read bytes with timeout ----------------
    def read_bytes(self, length, timeout_ms=500):
        buf = bytearray()
        start = time.time() * 1000
        while len(buf) < length:
            b = self.ser.read(1)
            if b:
                buf += b
            if (time.time()*1000 - start) > timeout_ms:
                return None
        return buf

    # ---------------- concentration ----------------
    def get_concentration(self):
        cmd = bytearray([ENV_X6_CMD_GET_CONC])
        cmd.append(self.checksum_0_add8(cmd))
        self.ser.write(cmd)
        rx = self.read_bytes(22, 500)
        if not rx or self.checksum_0_add8(rx[:21]) != rx[21]:
            return None
        data = {}
        data['iaq']  = self.bytes_to_float(rx[1:5])
        data['tvoc'] = self.bytes_to_float(rx[5:9])
        data['hcho'] = self.bytes_to_float(rx[9:13])
        data['co']   = self.bytes_to_float(rx[13:17])
        data['temperature'] = (rx[17]<<8 | rx[18])/100
        data['humidity']    = (rx[19]<<8 | rx[20])/100
        return data

    # ---------------- sensor param ----------------
    def get_sensor_param(self, index):
        cmd = bytearray([ENV_X6_CMD_GET_SENSOR, index])
        cmd.append(self.checksum_0_add8(cmd))
        self.ser.write(cmd)
        rx = self.read_bytes(7,500)
        if not rx or self.checksum_0_add8(rx[:6]) != rx[6]:
            return None
        return {
            'type': rx[2],
            'range': rx[3]<<8 | rx[4],
            'unit': rx[5],
            'name': GAS_NAME.get(rx[2],"Unknown")
        }

    # ---------------- LED ----------------
    def get_led(self):
        cmd = bytearray([ENV_X6_CMD_GET_LED])
        cmd.append(self.checksum_0_add8(cmd))
        self.ser.write(cmd)
        rx = self.read_bytes(3,500)
        if not rx or self.checksum_0_add8(rx[:2]) != rx[2]:
            return None
        return rx[1]

    def set_led(self, status):
        cmd = bytearray([ENV_X6_CMD_SET_LED, status])
        cmd.append(self.checksum_0_add8(cmd))
        self.ser.write(cmd)
        rx = self.read_bytes(5,500)
        if not rx or self.checksum_0_add8(rx[:4]) != rx[4]:
            return False
        return True

    # ---------------- sleep / wakeup ----------------
    def sleep(self):
        cmd = bytearray([ENV_X6_CMD_SLEEP,0x73,0x6C,0x65,0x65,0x70])
        cmd.append(self.checksum_0_add8(cmd))
        self.ser.write(cmd)
        return self.read_bytes(5,500) is not None

    def wakeup(self):
        cmd = bytearray([ENV_X6_CMD_WAKEUP,0x61,0x77,0x61,0x6B,0x65])
        cmd.append(self.checksum_0_add8(cmd))
        self.ser.write(cmd)
        return self.read_bytes(5,500) is not None

    # ---------------- serial / version ----------------
    def get_serial(self):
        cmd = bytearray([ENV_X6_CMD_GET_SERIAL])
        cmd.append(self.checksum_0_add8(cmd))
        self.ser.write(cmd)
        rx = self.read_bytes(8,500)
        if not rx or self.checksum_0_add8(rx[:7]) != rx[7]:
            return None
        return ''.join(f"{b:02X}" for b in rx[1:7])

    def get_version(self):
        cmd = bytearray([ENV_X6_CMD_GET_VERSION])
        cmd.append(self.checksum_0_add8(cmd))
        self.ser.write(cmd)
        rx = self.read_bytes(21,500)
        if not rx or self.checksum_0_add8(rx[:20]) != rx[20]:
            return None
        return rx[1:20].decode('ascii')
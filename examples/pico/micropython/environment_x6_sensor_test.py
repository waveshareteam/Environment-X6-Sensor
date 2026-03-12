from machine import UART, Pin
import struct
import time

# ----------------- UART CONFIG -----------------
UART_ID = 1
UART_TX = 4
UART_RX = 5
BAUDRATE = 9600

# ----------------- COMMANDS -----------------
CMD_GET_CONC = 0x70
CMD_GET_SENSOR = 0x72
CMD_GET_LED = 0x74
CMD_SET_LED = 0x56
CMD_SLEEP = 0x54
CMD_WAKEUP = 0x55
CMD_GET_SERIAL = 0x71
CMD_GET_VERSION = 0x73

# ----------------- GAS TYPES -----------------
HCHO = 0x17
TVOC = 0x18
CO   = 0x19
IAQ  = 0x33


class EnvironmentX6:

    def __init__(self):
        self.uart = UART(UART_ID, baudrate=BAUDRATE, tx=Pin(UART_TX), rx=Pin(UART_RX))

    # ----------------- checksum -----------------
    def checksum(self, data):
        return (-sum(data)) & 0xFF

    # ----------------- print raw rx -----------------
    def print_bytes(self, data):
        print("RX:", " ".join("{:02X}".format(b) for b in data))

    # ----------------- read bytes -----------------
    def read_bytes(self, length, timeout=500):
        start = time.ticks_ms()
        buf = b''

        while len(buf) < length:
            if self.uart.any():
                buf += self.uart.read(1)

            if time.ticks_diff(time.ticks_ms(), start) > timeout:
                return None

        return buf

    # ----------------- float convert -----------------
    def bytes_to_float(self, b):
        return struct.unpack(">f", b)[0]

    # ----------------- gas name -----------------
    def gas_name(self, t):
        if t == HCHO:
            return "HCHO"
        elif t == TVOC:
            return "TVOC"
        elif t == CO:
            return "CO"
        elif t == IAQ:
            return "IAQ"
        return "Unknown"

    # ----------------- read concentration -----------------
    def get_concentration(self):

        cmd = bytearray([CMD_GET_CONC, 0])
        cmd[1] = self.checksum(cmd[:1])

        self.uart.write(cmd)

        rx = self.read_bytes(22)
        if not rx:
            print("Read timeout")
            return None

#         self.print_bytes(rx)

        if self.checksum(rx[:21]) != rx[21]:
            print("Checksum error")
            return None

        data = {}

        data["iaq"] = self.bytes_to_float(rx[1:5])
        data["tvoc"] = self.bytes_to_float(rx[5:9])
        data["hcho"] = self.bytes_to_float(rx[9:13])
        data["co"] = self.bytes_to_float(rx[13:17])

        temp = (rx[17] << 8) | rx[18]
        hum  = (rx[19] << 8) | rx[20]

        data["temperature"] = temp / 100.0
        data["humidity"] = hum / 100.0

        return data

    # ----------------- sensor param -----------------
    def get_sensor_param(self, index):

        cmd = bytearray([CMD_GET_SENSOR, index, 0])
        cmd[2] = self.checksum(cmd[:2])

        self.uart.write(cmd)

        rx = self.read_bytes(7)
        if not rx:
            return None

#         self.print_bytes(rx)

        if self.checksum(rx[:6]) != rx[6]:
            print("Checksum error")
            return None

        return {
            "type": rx[2],
            "range": (rx[3] << 8) | rx[4],
            "unit": rx[5]
        }

    # ----------------- LED -----------------
    def get_led(self):

        cmd = bytearray([CMD_GET_LED, 0])
        cmd[1] = self.checksum(cmd[:1])

        self.uart.write(cmd)

        rx = self.read_bytes(3)
        if not rx:
            return None

        if self.checksum(rx[:2]) != rx[2]:
            return None

        return rx[1]

    def set_led(self, status):

        cmd = bytearray([CMD_SET_LED, status, 0])
        cmd[2] = self.checksum(cmd[:2])

        self.uart.write(cmd)

        rx = self.read_bytes(5)
        if not rx:
            return False

        return True
        
    # ----------------- sleep -----------------
    def sleep(self):

        cmd = bytearray([CMD_SLEEP, 0x73, 0x6C, 0x65, 0x65, 0x70, 0])
        cmd[6] = self.checksum(cmd[:6])

        self.uart.write(cmd)

        rx = self.read_bytes(5)
        if not rx:
            return False

#         self.print_bytes(rx)
        return True
    
    # ----------------- wakeup -----------------
    def wakeup(self):

        cmd = bytearray([CMD_WAKEUP, 0x61, 0x77, 0x61, 0x6B, 0x65, 0])
        cmd[6] = self.checksum(cmd[:6])

        self.uart.write(cmd)

        rx = self.read_bytes(5)
        if not rx:
            return False

#         self.print_bytes(rx)
        return True
    
    # ----------------- serial number -----------------
    def get_serial(self):

        cmd = bytearray([CMD_GET_SERIAL, 0])
        cmd[1] = self.checksum(cmd[:1])

        self.uart.write(cmd)

        rx = self.read_bytes(8)
        if not rx:
            return None

#         self.print_bytes(rx)

        if self.checksum(rx[:7]) != rx[7]:
            print("Checksum error")
            return None

        return "".join("{:02X}".format(b) for b in rx[1:7])
    
    # ----------------- software version -----------------
    def get_version(self):

        cmd = bytearray([CMD_GET_VERSION, 0])
        cmd[1] = self.checksum(cmd[:1])

        self.uart.write(cmd)

        rx = self.read_bytes(21)
        if not rx:
            return None

#         self.print_bytes(rx)

        if self.checksum(rx[:20]) != rx[20]:
            print("Checksum error")
            return None

        return rx[1:20].decode().strip("\x00")


TEST_CONCENTRATION = 1
TEST_SENSOR_PARAM = 0
TEST_LED = 0
TEST_SLEEP_WAKEUP = 0
TEST_SERIAL = 0
TEST_VERSION = 0

sensor = EnvironmentX6()

print("=== iNose X6 Test Start ===")

while True:

    if TEST_CONCENTRATION:
        data = sensor.get_concentration()
        if data:
            print("----- Concentration -----")
            print("HCHO:", round(data["hcho"],3))
            print("TVOC:", round(data["tvoc"],3))
            print("CO:", round(data["co"],3))
            print("IAQ:", round(data["iaq"],3))
            print("Temp:", data["temperature"])
            print("Hum:", data["humidity"])

    if TEST_SENSOR_PARAM:

        for gas in [HCHO, TVOC, CO, IAQ]:

            param = sensor.get_sensor_param(gas)

            if param:
                print("---- Sensor Param ----")
                print("Gas:", sensor.gas_name(param["type"]))
                print("Type: 0x{:02X}".format(param["type"]))
                print("Range:", param["range"])
                print("Unit:", param["unit"])

    if TEST_LED:

        led = sensor.get_led()

        if led is not None:
            print("LED:", "ON" if led else "OFF")
            sensor.set_led(0 if led else 1)
            
    if TEST_SLEEP_WAKEUP:
        sensor.sleep()
        time.sleep(1)
        sensor.wakeup()

    if TEST_SERIAL:
        sn = sensor.get_serial()
        if sn:
            print("Serial:", sn)

    if TEST_VERSION:
        ver = sensor.get_version()
        if ver:
            print("Version:", ver)
            
    print("=== Loop End ===\n\n")

    time.sleep(2)
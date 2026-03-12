#coding: UTF-8
import sys 
from time import sleep
sys.path.append("..")
from lib import environment_x6

# ----------------- Feature switches -----------------
TEST_CONCENTRATION    = True
TEST_SENSOR_PARAM     = False
TEST_LED              = False
TEST_SLEEP_WAKEUP     = False
TEST_SERIAL           = False
TEST_VERSION          = False

# ----------------- Detect Raspberry Pi model -----------------
def detect_model():
    """
    Detects the Raspberry Pi model by reading the device tree model file.
    
    Returns:
        str: The model string of the Raspberry Pi.
    """
    with open('/proc/device-tree/model') as f:
        model = f.read().strip()
    return model

# ----------------- Initialize X6 sensor -----------------
if "Raspberry Pi 5" in detect_model() or "Raspberry Pi Compute Module 5" in detect_model():
    env = environment_x6.EnvironmentX6('/dev/ttyAMA0', 9600)
else:
    env = environment_x6.EnvironmentX6('/dev/ttyS0', 9600)

# ----------------- Print helpers -----------------
def print_concentration(data):
    print("--- Concentration ---")
    print(f"IAQ: {data['iaq']:.1f}")
    print(f"TVOC: {data['tvoc']:.3f} ppm")
    print(f"HCHO: {data['hcho']:.3f} ppm")
    print(f"CO: {data['co']:.3f} ppm")
    print(f"Temperature: {data['temperature']:.2f} °C")
    print(f"Humidity: {data['humidity']:.2f} %")

def print_sensor_param(param):
    print(f"--- Sensor Param ({param['name']}) ---")
    print(f"Type: 0x{param['type']:02X}")
    print(f"Range: {param['range']}")
    print(f"Unit: {param['unit']}")

# ----------------- Main loop -----------------
if __name__ == "__main__":
    try:
        # 唤醒设备
        env.wakeup()
        sleep(0.2)

        while True:
            if TEST_CONCENTRATION:
                data = env.get_concentration()
                if data:
                    print_concentration(data)
                else:
                    print("Failed to read concentration")

            if TEST_SENSOR_PARAM:
                for i in range(4):
                    param = env.get_sensor_param(i)
                    if param:
                        print_sensor_param(param)
                    else:
                        print(f"Failed to read sensor param for index {i}")

            if TEST_LED:
                led = env.get_led()
                if led is not None:
                    print(f"LED: {'ON' if led else 'OFF'}")
                else:
                    print("Failed to read LED status")

            if TEST_SLEEP_WAKEUP:
                print("Putting sensor to sleep for 1s...")
                env.sleep()
                sleep(1)
                print("Waking up sensor...")
                env.wakeup()

            if TEST_SERIAL:
                serial = env.get_serial()
                print(f"Serial: {serial if serial else 'N/A'}")

            if TEST_VERSION:
                version = env.get_version()
                print(f"Version: {version if version else 'N/A'}")

            print()  # Blank line for readability
            sleep(1)

    except KeyboardInterrupt:
        print("\nUser interrupted, exiting safely...")
    finally:
        env.ser.close()
from machine import Pin, PWM
import time
import network
import logging
from arduino_iot_cloud import ArduinoCloudClient

from secrets import WIFI_SSID
from secrets import WIFI_PASSWORD
from secrets import DEVICE_ID
from secrets import CLOUD_PASSWORD

GATE_OPEN = 0
GATE_CLOSED = 90

led = Pin(6, Pin.OUT) # Configure the desired LED pin as an output.

redLightPin = Pin(16, mode = Pin.OUT)
greenLightPin = Pin(17, mode = Pin.OUT)

servoPin = Pin(15, mode=Pin.OUT)
servo = PWM(servoPin)
duty_cycle = 0 #  Define and initialise duty cycle PWM
offset = 60 # servo mounting position offset
servo.freq(50)


# Define function to set servo angle
def set_servo(angle):
    angle = angle + offset
    duty_cycle = int(angle*(7803-1950)/180) + 1950
    servo.duty_u16(duty_cycle)

def on_region_changed(client, value):
  print(value)
  if value == "EU" or value == "SWISS":
    print("Opening barrier")
    greenLightOn(True)
    set_servo(GATE_OPEN)
  else:
    print("Closing barrier")
    greenLightOn(False)
    set_servo(GATE_CLOSED)
    
def greenLightOn(value):
  if value:
    greenLightPin.value(1)
    led.value(1)
    redLightPin.value(0)

  else:
    greenLightPin.value(0)
    led.value(0)
    redLightPin.value(1)
    
  

def wifi_connect():
    if not WIFI_SSID or not WIFI_PASSWORD:
        raise (Exception("Network is not configured. Set SSID and passwords in secrets.py"))
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(WIFI_SSID, WIFI_PASSWORD)
    while not wlan.isconnected():
        logging.info("Trying to connect. Note this may take a while...")
        time.sleep_ms(500)
    logging.info(f"WiFi Connected {wlan.ifconfig()}")

if __name__ == "__main__":
    # Configure the logger.
    # All message equal or higher to the logger level are printed.
    # To see more debugging messages, set level=logging.DEBUG.
    logging.basicConfig(
        datefmt="%H:%M:%S",
        format="%(asctime)s.%(msecs)03d %(message)s",
        level=logging.INFO,
    )

    # NOTE: Add networking code here or in boot.py
    set_servo(GATE_CLOSED)
    greenLightOn(False)
    wifi_connect()

    # Create a client object to connect to the Arduino Cloud.
    # For MicroPython, the key and cert files must be stored in DER format on the filesystem.
    # Alternatively, a username and password can be used to authenticate:
    client = ArduinoCloudClient(device_id=DEVICE_ID, username=DEVICE_ID, password=CLOUD_PASSWORD)

    # Register Cloud objects.
    # Note: The following objects must be created first in the dashboard and linked to the device.
    # This Cloud object is initialized with its last known value from the Cloud. When this object is updated
    # from the dashboard, the on_switch_changed function is called with the client object and the new value.
    # client.register("ledSwitch", value=None, on_write=on_switch_changed, interval=0.250)

    # client.register("ledSwitch", value=None, on_write=on_region_changed, interval=0.250)
    
    client.register("region", value=None, on_write=on_region_changed)
    # This Cloud object is updated manually in the switch's on_write_change callback to update the LED state in the Cloud.
    # client.register("led", value=None)

    # Start the Arduino Cloud client.
    client.start()
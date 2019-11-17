#  oled.py  Display useful info on 0.96" OLED display
#  Written by mosquito@darlingevil.com, 2019-11-15

import os
import time
from datetime import datetime
import board
import digitalio
import subprocess
from PIL import Image, ImageDraw, ImageFont
import adafruit_ssd1306

# These are pulled in from the host when the container is run
LOCAL_ROUTER_ADDRESS  = os.environ['LOCAL_ROUTER_ADDRESS']
LOCAL_IP_ADDRESS      = os.environ['LOCAL_IP_ADDRESS']

# Commands to check LAN, WAN, etc.
LAN_COMMAND = 'curl -sS https://' + LOCAL_ROUTER_ADDRESS + ' 2>/dev/null | wc -l'
WAN_COMMAND = 'curl -sS https://google.com 2>/dev/null | wc -l'
UPTIME_COMMAND = "uptime | awk '{printf \"up %s avg %.2f\", $3, $(NF-2)}'"

# Define the Reset Pin
oled_reset = digitalio.DigitalInOut(board.D4)

# Change these
# to the right size for your display!
WIDTH = 128
HEIGHT = 64
BORDER = 5

# Use for I2C.
i2c = board.I2C()
oled = adafruit_ssd1306.SSD1306_I2C(WIDTH, HEIGHT, i2c, addr=0x3c, reset=oled_reset)

# Clear display.
oled.fill(0)
oled.show()

# Create blank image for drawing.
# Make sure to create image with mode '1' for 1-bit color.
#image = Image.new('1', (oled.width, oled.height))

# Get drawing object to draw on image.
# Create blank image for drawing.
# Make sure to create image with mode '1' for 1-bit color.
image = Image.new('1', (oled.width, oled.height))
draw = ImageDraw.Draw(image)

# Load default font.
font = ImageFont.load_default()

# Draw text at coordinates
def text_xy(x, y, text):
  draw.text((x, y), text, font=font, fill=255)

# Draw center-aligned text
def text_centered_y(y, text):
  (font_width, font_height) = font.getsize(text)
  draw.text((oled.width//2 - font_width//2, y), text, font=font, fill=255)

while (True):

  # Draw a black background
  draw.rectangle((0, 0, oled.width, oled.height), outline=0, fill=0)

  lan = '0' != str(subprocess.check_output(LAN_COMMAND, shell=True)).strip()
  wan = '0' != str(subprocess.check_output(WAN_COMMAND, shell=True)).strip()

  text_centered_y(2, "IPv4: " + LOCAL_IP_ADDRESS)
  if lan:
    text_xy(0, 14, "Gateway: (connected)")
  else:
    text_xy(0, 14, "Gateway: UNREACHABLE!")
  if wan:
    text_xy(0, 24, "Internet: (reachable)")
  else:
    text_xy(0, 24, "Internet: UNREACHABLE!")

  text_xy(0, 34, " ")

  date = datetime.utcnow().strftime("UTC: %H:%M:%S")
  text_centered_y(44, date)
  uptime = subprocess.check_output(UPTIME_COMMAND, shell=True)
  uptime = uptime.decode("utf-8").strip()
  text_centered_y(54, "" + uptime)

  # Display image
  oled.image(image)
  oled.show()

  time.sleep(5)



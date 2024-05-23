![](WiFi-2-OIP-UI.jpg)

# Status

Unfinished. Untested.

# What?

This is a PCB that connects an ESP32 to a reclaimed display board from an obsolete [Octalarm-IP](https://www.adesys.nl/en/product-octalarm-ip-adesys2).

# BOM

See: [Kicad/bom/ibom.html](Kicad/bom/ibom.html)

## BOM remarks

U1 should be a 5V buck converter in the footprint of a TO-220. It should be capable of outputting 5V at 1A. Input voltage should be greater than 12V.

J201 are 2 pieces of 1x05 2.54mm pp low profile (7.5mm) pin header instead of a Micro-MaTch connector.

For connecting TP203 you need 2cm of red wire.

The ESP32-S2 should come with 2 pieces of 2x08 2.54mm pin headers. Discard them and order 4 pieces of 1x08 2.54mm pp low profile (7.5mm) pin headers.

J201 Phoenix contact 1803277+1851999 can also be 1803277+1803578.

# Soldering and assembly instructions

Solder the pin headers on the ESP32-S2, use some old IDC connectors to perfectly align the headers. Like this:

![](esp32-s2-idc.jpg)

Solder F1, D1.  Solder U1 (the buck converter) laying flat (PCBs are now on the same plane). Cut pins (6 cuts) of U1 flush with PCB. Solder J2, C1 and C2. Cut pins of J2 flush with PCB. Solder one end of the red wire to TP203 on the bottom. The right side of the PCB now look like this:

![](WiFi-2-OIP-UI-flat-U1.jpg)

Set your lab power supply to 12V CV, 0.25A CC. Then connect 12V to J2. Observe less than 10mA current. Measure 5V between TP203 and ground. Disconnect power supply.

Passed? Now solder A1 (the ESP32-S2 module). Did not you use low profile pin header then cut pins flush with PCB. Works best before soldering.

Connect power supply again. Observe about 40mA current. Disconnect power supply.

Desolder the J201 MicroMatch 1.27mm socket from the display board.

Gently clean the PCB and the ESP32-S2 if you want.

Connect this PCB to the back of the display board using 2 pieces of 1x5 2.54mm pin headers in the J201 footprints of both PCBs. The hole J202 on both PCB should almost align. But do not solder the header yet.
First solder a 1x02 2.54mm pin header on J202 of both PCBs to keep PCBs separated. Now solder the J201 pin header.

Solder the red wire onto the TP203 pad of the display board.

Connect power supply again. Observe about 50mA current. Disconnect power supply.

Make sure none of the pins (ESP32-S2, U1, J2) of this PCB touch display board:

![](WiFi-2-OIP-UI-side.jpg)

# Firmware

Flash the firmware of the ESP32-S2. The firmware of the ATmega48 MCU of the display board is embedded in the  ESP32-S2 firmware and will be flashed by the ESP32-S2.

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
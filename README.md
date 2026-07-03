# use_BLE_esp32 - Automatic attaching to BLE HID keyboard/mouse/gamepad

This project is the beginning of an Arduino library to support receiving input reports from your BLE HID devices.

Scans until finds a device to connect with (must be in pairing mode), connects, and registers for input reports.  If disconnects, then goes back into scanning mode.

Input reports are *conditionally* CBM keyboard scan codes, or unparsed byte arrays (other or all HID devices).  Mouse/trackpad reports should be pretty standard.  Gamepad reports will require additional parsing of HID map (warning: not yet exposed to upper interface, nor interpreted yet).

It is up to the user of the library to interpret the input reports into keys pressed, buttons pressed, hat/gamepad position, etc.

The library is a work in progress and may not suit all your needs, and have other limitations.  It is currently targeting a few M5Stack devices, and generic ESP32-C3.  Build environment expects platformio (for stricter versioning).

Code is based on my [BLEcli](https://github.com/davervw/BLEcli) that is interactive and requires multiple steps.  In contrast this code is rapid, with automatic connection.

````
void hidReport(size_t len, uint8_t *data, bool isCBM);

use_BLEHID_esp  Copyright (c) 2026 David R. Van Wagner  MIT LICENSE  davevw.com  https://github.com/davervw

00 00 04 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 05 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 06 00 00 00 00 00
00 00 00 00 00 00 00 00

Failed to receive HID map
00 FD 50 00
00 01 80 00
00 01 80 03
05 01 80 00
05 01 80 05
01 01 80 00
01 01 80 07
00 01 80 00
00 01 80 03
01 01 80 03
Disconnected

Failed to receive HID map
15,33
88
2
88
15,7
88
Disconnected
````

BLE Devices tested:

* [Lenovo TrackPoint Keyboard II](https://support.lenovo.com/us/en/accessories/acc500164-thinkpad-trackpoint-keyboard-ii-overview-and-service-parts)
* [XBox One Controller](https://www.xbox.com/en-US/accessories/controllers/xbox-wireless-controller)
* [Meetion BLE Foldable Keyboard K9552](https://www.amazon.com/MEETION-Bluetooth-Multi-Device-Keyboards-Full-Size/dp/B0GLXS4SVX)
* [IINE GameBrick Mini Retro Controller](https://iine.store/products/iine-gamebrick-mini-retro-controller) (Play My Way) in XINPUT mode
* [M5MiniJoyC Gamepad](https://github.com/davervw/m5_minijoystickc_gamepad)
* [KanoPixelKit Gamepad](https://github.com/davervw/ble_gamepad_KanoPixelKit)
* [Cardputer M5-Keyboard](https://github.com/m5stack/M5Cardputer-UserDemo) (requires [Pull Request #44] or later (https://github.com/davervw/M5Cardputer-UserDemo/tree/davervw-ble-keyboard-fixes))
* [BLE adapter for Palm Portable Keyboard (rev3)](https://github.com/pymo/ppk_bluetooth)
* [My revisions to make Commodore version of Palm Portable Keyboard](https://github.com/davervw/ppk_bluetooth_for_cbm)
* [My custom Commodore BLE Keyboard Adapter](https://github.com/davervw/c-simple-emu6502-cbm/tree/unified/src/BLE_commodore_keyboard_server)

# use_BLE_esp32 - Automatic attaching to BLE HID keyboard/mouse/gamepad

This project is the beginning of an Arduino library to support receiving input reports from your BLE HID devices.

Scans until finds a device to connect with (must be in pairing mode), connects, and registers for input reports.  If disconnects, then goes back into scanning mode.

Input reports are unparsed length plus byte arrays.  Keyboard/mouse reports should be pretty standard.  Gamepad reports will require additional parsing of HID map (warning: not yet exposed to upper interface, nor interpreted yet).

It is up to the user of the library to interpret the input reports into keys pressed, buttons pressed, hat/gamepad position, etc.

The library is a work in progress and may not suit all your needs, and have other limitations.  It is currently targeting M5Stack devices, but can be revised to work with generic ESP32.  Build environment expects platformio (for stricter versioning).

Code is based on my [BLEcli](https://github.com/davervw/BLEcli) which is interactive, while this library has rapid, automatic connections.

````
void hidReport(size_t len, uint8_t *data);

FOUND Keyboard
len=8 00 00 04 00 00 00 00 00
len=8 00 00 00 00 00 00 00 00
len=8 00 00 05 00 00 00 00 00
len=8 00 00 00 00 00 00 00 00
len=8 00 00 06 00 00 00 00 00
len=8 00 00 00 00 00 00 00 00
len=2 B5 00
len=2 00 00
len=4 01 00 00 00
len=4 00 00 00 00
len=4 00 FF 00 00
len=4 00 FF 00 00
Disconnected

FOUND Gamepad
Failed to receive HID map
len=4 00 FD 50 00
len=4 00 01 80 00
len=4 00 01 80 03
len=4 05 01 80 00
len=4 05 01 80 05
len=4 01 01 80 00
len=4 01 01 80 07
len=4 00 01 80 00
len=4 00 01 80 03
len=4 01 01 80 03
Disconnected

````

BLE Devices tested:

* [Lenovo TrackPoint Keyboard II](https://support.lenovo.com/us/en/accessories/acc500164-thinkpad-trackpoint-keyboard-ii-overview-and-service-parts)
* [XBox One Controller](https://www.xbox.com/en-US/accessories/controllers/xbox-wireless-controller)
* [Meetion BLE Foldable Keyboard K9552](https://www.amazon.com/MEETION-Bluetooth-Multi-Device-Keyboards-Full-Size/dp/B0GLXS4SVX)
* [IINE GameBrick Mini Retro Controller](https://iine.store/products/iine-gamebrick-mini-retro-controller) (Play My Way) in XINPUT mode
* [M5MiniJoyC Gamepad](https://github.com/davervw/m5_minijoystickc_gamepad)
* [KanoPixelKit Gamepad](https://github.com/davervw/ble_gamepad_KanoPixelKit)
* [Cardputer M5-Keyboard](https://github.com/m5stack/M5Cardputer-UserDemo) (FAIL - reports generic HID type, possible to relax code constrainst)

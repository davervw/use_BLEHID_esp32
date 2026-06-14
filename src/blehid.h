#include <Arduino.h>

class cBLEHID {
    public:
        void init();
        void scan(void (*scanResult)(bool found), uint32_t durationMs);
        bool connect();
        void disconnect();
        std::vector<uint8_t> getHIDmap();
        bool listenReports(
            void (*keyboardReport)(size_t len, uint8_t* data),
            void (*gamepadReport)(size_t len, uint8_t* data)
            );
        bool isConnected();
        bool isScanning();
        bool isKeyboard();
        bool isGamePad();
};

extern cBLEHID BLEHID;
//////////////////////////////////////////////////////////////////////
// blehid.h
//
// use_BLEHID_esp32 - auto pairing with BLE Keyboard, Mouse, Gamepad
// Copyright (c) 2026 David R. Van Wagner
//
// MIT LICENSE
//
// https://davevw.com
// https://github.com/davervw
//////////////////////////////////////////////////////////////////////

#include <vector>

class cBLEHID {
    public:
        void init();
        void scan(void (*scanResult)(bool found), uint32_t durationMs);
        bool connect(void (*disconnected)());
        void disconnect();
        std::vector<uint8_t> getHIDmap();
        bool listenReports(void (*hidReport)(size_t len, uint8_t* data));
        bool isConnected();
        bool isScanning();
        bool isKeyboard();
        bool isGamePad();
};

extern cBLEHID BLEHID;
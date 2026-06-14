//////////////////////////////////////////////////////////////////////
// autoblehid.h
//
// use_BLEHID_esp32 - auto pairing with BLE Keyboard, Mouse, Gamepad
// Copyright (c) 2026 David R. Van Wagner
//
// MIT LICENSE
//
// https://davevw.com
// https://github.com/davervw
//////////////////////////////////////////////////////////////////////

class AutoBleHid
{
public:
    void begin(void (*hidReport)(size_t length, uint8_t* data));
    void update();
};

extern AutoBleHid AUTOBLEHID;
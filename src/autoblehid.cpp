//////////////////////////////////////////////////////////////////////
// autoblehid.cpp
//
// use_BLEHID_esp32 - auto pairing with BLE Keyboard, Mouse, Gamepad
// Copyright (c) 2026 David R. Van Wagner
//
// MIT LICENSE
//
// https://davevw.com
// https://github.com/davervw
//////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "autoblehid.h"
#include "blehid.h"

AutoBleHid AUTOBLEHID;

// forward declaration
static void doScan();

static bool do_connect = false;
static void (*_hidReport)(size_t length, uint8_t* data) = nullptr;

static void scanResult(bool found)
{
    // Serial.println("Scanning finished.");
#if (CORE_DEBUG_LEVEL >= 3)    
    Serial.printf(found ? "FOUND %s\n" : "NOT FOUND\n", BLEHID.isKeyboard() ? "Keyboard" : BLEHID.isGamePad() ? "Gamepad" : "");
#endif    
    if (found)
        do_connect = true;
}

static void doScan()
{
    // Serial.println("Scanning...");
    BLEHID.scan(&scanResult, 0);
}

static void onDisconnected()
{
    Serial.println("Disconnected");
    doScan();
}

static void doConnect()
{
    // Serial.println("Connecting...");
    if (BLEHID.connect(&onDisconnected))
    {
        // Serial.println("Connected to device.");
        auto map = BLEHID.getHIDmap();
        // Serial.printf("HID map size = %ld\n", (long)map.size());
        if (map.size() == 0)
            Serial.println("Failed to receive HID map");
        if (BLEHID.listenReports(_hidReport))
        {
            // Serial.println("Listening for reports");
        }
        else
        {
            Serial.println("Failed to listen for reports");
        }
    }
}

void AutoBleHid::begin(void (*hidReport)(size_t length, uint8_t* data))
{
    _hidReport = hidReport;
    BLEHID.init();
    doScan();
}

void AutoBleHid::update()
{
    if (do_connect)
    {
        do_connect = false;
        doConnect();
    }
}

//////////////////////////////////////////////////////////////////////
// main.cpp
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
#ifdef M5STACK
#include <M5Unified.h>
#endif
#include <WiFi.h>
#include "autoblehid.h"

void hidReport(size_t len, uint8_t *data)
{
#if (CORE_DEBUG_LEVEL >= 3)
    Serial.printf("len=%d ", len);
#endif    
    for (auto i = 0; i < len; ++i)
    {
        if (i > 0)
            Serial.print(' ');
        Serial.printf("%02X", data[i]);
    }
    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 2000)
    {
        delay(10);
    }

    WiFi.disconnect();
    delay(100);
    WiFi.mode(WIFI_OFF);

#ifdef M5STACK
    M5.begin();
    M5.Display.setRotation(1);
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("use_BLEHID", M5.Display.width() / 2, M5.Display.height() / 2);
#endif    

    AUTOBLEHID.begin(&hidReport);
}

void loop()
{
#ifdef M5STACK    
    M5.update();
#endif    
    AUTOBLEHID.update();

    delay(10);
}

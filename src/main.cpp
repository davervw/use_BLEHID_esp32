//////////////////////////////////////////////////////////////////////
// main.cpp
//
// use_BLEHID_esp32 - auto pairing with BLE Keyboard, Mouse, Gamepad
// Copyright (c) 2026 David R. Van Wagner
//
// https://davevw.com
// https://github.com/davervw
//
//////////////////////////////////////////////////////////////////////
//
// MIT License
//
// Copyright (c) 2026 by David R. Van Wagner
// davevw.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#ifdef M5STACK
#include <M5Unified.h>
#endif
#include <WiFi.h>
#include "autoblehid.h"
#include "blehid.h"
#include "HIDtoCBMkeyboard.h"

// default is to output HID byte reports for more global use
// here is an option to output Commodore scan codes that is mostly useful for my own emulators; it's just for me, so default is off
const bool COMMODORE_OUTPUT = false; // true to convert BLE keyboards to output CBM keyboard scan codes, otherwise HID bytes for generic use
HIDtoCBMkeyboard hidcbm;

void hidReport(size_t len, uint8_t *data, bool isCBM)
{
#if (CORE_DEBUG_LEVEL >= 3)
    Serial.printf("len=%d ", len);
#endif
    if (isCBM) { // Commodore keyboards will always output in Commodore format
        Serial.print(String(data, len));
        return;
    }

bool isKeyboard = BLEHID.isKeyboard();
#if (CORE_DEBUG_LEVEL >= 3)
    bool outputHex = true;
#else
    bool outputHex = !isKeyboard || !COMMODORE_OUTPUT;
#endif
    if (outputHex) {
        for (auto i = 0; i < len; ++i)
        {
            if (i > 0)
                Serial.print(' ');
            Serial.printf("%02X", data[i]);
        }
        Serial.println();
    }

    if (!isKeyboard)
        return;

    if (!COMMODORE_OUTPUT)
        return;

    hidcbm.OnKeyData(len, data);
    String s = hidcbm.Read();
    if (s.length() != 0)
        Serial.print(s);
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
    Serial.println("use_BLEHID_esp  Copyright (c) 2026 David R. Van Wagner  MIT LICENSE  davevw.com  https://github.com/davervw");    

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

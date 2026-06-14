#include <M5Unified.h>
#include "blehid.h"

// forward declaration
void doScan();

void keyboardReport(uint8_t len, uint8_t* data)
{
    Serial.printf("len=%d", len);
    for (int i=0; i<len; ++i)
        Serial.printf(" %02X", data[i]);
    Serial.println();
}

void mouseReport(uint8_t len, uint8_t* data)
{

}

void gamepadReport(uint8_t len, uint8_t* data)
{

}


void scanResult(bool found)
{
    Serial.println("Scanning finished.");
    Serial.printf(found ? "FOUND\n" : "NOT FOUND\n");
    if (!found)
        return;
}

void doScan()
{
    Serial.println("Scanning...");
    BLEHID.scan(&scanResult, 7500);
}

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 2000)
    {
        delay(10);
    }

    M5.begin();
    M5.Display.setRotation(1);
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("use_BLEHID", M5.Display.width() / 2, M5.Display.height() / 2);

    BLEHID.init();
    doScan();
}

void loop()
{
    M5.update();

    static bool connect_attempt = false;
    if (!BLEHID.isScanning() && !BLEHID.isConnected() && BLEHID.isKeyboard() && !connect_attempt)
    {
        connect_attempt = true;
        Serial.println("Connecting...");
        if (BLEHID.connect()) 
        {
            Serial.println("Connected to keyboard.");
            auto map = BLEHID.getHIDmap();
            Serial.printf("HID map size = %ld", (long)map.size());
            if (BLEHID.listenReports(&keyboardReport, &mouseReport, &gamepadReport))
                Serial.println("Listening for reports");
            else
                Serial.println("Failed to listen for reports");
        }
    }

    // static int count = 0;
    // if (++count > 100)
    // {
    //     count = 0;
    //     Serial.printf("connected=%d, scanning=%d, keyboard=%d, mouse=%d, gamepad=%d\n",
    //         BLEHID.isConnected(),
    //         BLEHID.isScanning(),
    //         BLEHID.isKeyboard(),
    //         BLEHID.isMouse(),
    //         BLEHID.isGamePad()
    //     );
    // }

    delay(10);
}

#include <M5Unified.h>
#include "blehid.h"

// forward declaration
void doScan();

bool do_connect = false;

void hidReport(size_t len, uint8_t *data)
{
    Serial.printf("len=%d", len);
    for (auto i = 0; i < len; ++i)
        Serial.printf(" %02X", data[i]);
    Serial.println();
}

void scanResult(bool found)
{
    // Serial.println("Scanning finished.");
    Serial.printf(found ? "FOUND\n" : "NOT FOUND\n");
    if (found)
        do_connect = true;
}

void doScan()
{
    // Serial.println("Scanning...");
    BLEHID.scan(&scanResult, 7500);
}

void doConnect()
{
    // Serial.println("Connecting...");
    if (BLEHID.connect())
    {
        // Serial.println("Connected to device.");
        auto map = BLEHID.getHIDmap();
        // Serial.printf("HID map size = %ld\n", (long)map.size());
        if (map.size() == 0)
            Serial.println("Failed to receive HID map");
        if (BLEHID.listenReports(&hidReport))
        {
            // Serial.println("Listening for reports");
        }
        else
        {
            Serial.println("Failed to listen for reports");
        }
    }
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
    M5.Display.setTextSize(2);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString("use_BLEHID", M5.Display.width() / 2, M5.Display.height() / 2);

    BLEHID.init();
    doScan();
}

void loop()
{
    M5.update();

    if (do_connect)
    {
        do_connect = false;
        doConnect();
    }

    delay(10);
}

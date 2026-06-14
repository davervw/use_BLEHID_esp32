#include <M5Unified.h>
#include "autoblehid.h"

void hidReport(size_t len, uint8_t *data)
{
    Serial.printf("len=%d", len);
    for (auto i = 0; i < len; ++i)
        Serial.printf(" %02X", data[i]);
    Serial.println();
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

    AUTOBLEHID.begin(&hidReport);
}

void loop()
{
    M5.update();
    AUTOBLEHID.update();

    delay(10);
}

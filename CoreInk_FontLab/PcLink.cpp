#include "PcLink.h"

#include <M5Unified.h>
#include <cstring>

extern M5GFX display;

const uint32_t kAckTimeoutMs = 18000;

bool checkScriptReady()
{
  if (!Serial.available())
    return false;
  String line = Serial.readStringUntil('\n');
  line.trim();
  return line == "READY";
}

bool waitForAck(uint32_t timeoutMs)
{
  String line;
  uint32_t start = millis();
  while (millis() - start < timeoutMs)
  {
    if (Serial.available())
    {
      line = Serial.readStringUntil('\n');
      line.trim();
      if (line == "ACK")
        return true;
    }
  }
  return false;
}

void haltWithError(const String &msg)
{
  display.setEpdMode(epd_mode_t::epd_fast);
  display.fillScreen(TFT_WHITE);
  display.setTextColor(TFT_BLACK);
  display.setFont(&fonts::DejaVu18);
  display.setTextSize(1, 1);
  display.setCursor(0, 0);
  display.setTextWrap(true, false);
  display.println("ERROR");
  display.println(msg);
  display.println();
  display.println("Reset to run again.");
  display.display();

  Serial.flush();
  delay(200);
  M5.Power.powerOff();
}

void sendPageAndWaitAck(const String &name)
{
  const int32_t w = display.width();
  const int32_t h = display.height();
  const int32_t rowBytes = w * 3;
  const int32_t rowPadded = (rowBytes + 3) & ~3;
  const uint32_t dataSize = (uint32_t)rowPadded * h;
  const uint32_t fileSize = 54 + dataSize;

  uint8_t *row = (uint8_t *)malloc(rowPadded);
  if (!row)
    return;
  memset(row + rowBytes, 0, rowPadded - rowBytes);

  uint8_t header[54] = {0};
  header[0] = 'B';
  header[1] = 'M';
  header[2] = (uint8_t)fileSize;
  header[3] = (uint8_t)(fileSize >> 8);
  header[4] = (uint8_t)(fileSize >> 16);
  header[5] = (uint8_t)(fileSize >> 24);
  header[10] = 54;
  header[14] = 40;
  header[18] = (uint8_t)w;
  header[19] = (uint8_t)(w >> 8);
  header[20] = (uint8_t)(w >> 16);
  header[21] = (uint8_t)(w >> 24);
  header[22] = (uint8_t)h;
  header[23] = (uint8_t)(h >> 8);
  header[24] = (uint8_t)(h >> 16);
  header[25] = (uint8_t)(h >> 24);
  header[26] = 1;
  header[28] = 24;
  header[34] = (uint8_t)dataSize;
  header[35] = (uint8_t)(dataSize >> 8);
  header[36] = (uint8_t)(dataSize >> 16);
  header[37] = (uint8_t)(dataSize >> 24);

  Serial.println();
  Serial.print("NAME:");
  Serial.println(name);
  Serial.println("BEGIN_BMP");
  Serial.write(header, sizeof(header));

  for (int32_t y = h - 1; y >= 0; --y)
  {
    display.readRectRGB(0, y, w, 1, row);
    Serial.write(row, rowPadded);
  }

  Serial.println();
  Serial.println("END_BMP");

  free(row);

  if (!waitForAck(kAckTimeoutMs))
  {
    haltWithError("SCRIPT on PC is not responding via the Serial Port.");
  }
}

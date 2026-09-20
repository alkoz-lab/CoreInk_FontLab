#pragma once

#include <M5Unified.h>
#include <M5GFX.h>

#include "font_layout.h"
#include "browser_ui.h"
#include "capture.h"

extern M5GFX display;

static bool checkScriptReady()
{
  if (!Serial.available())
    return false;
  String line = Serial.readStringUntil('\n');
  line.trim();
  return line == "READY";
}

static void showInstructions()
{
  display.setEpdMode(epd_mode_t::epd_fast);
  display.setFont(&fonts::AsciiFont8x16);
  display.setAutoDisplay(false);
  display.setTextSize(1, 1);
  display.setCursor(0, 0);
  display.println("   CoreInk Font Lab");
  String underline;
  for (int i = 0; i < 25; ++i)
    underline += "Ì";
  display.println(underline);
  display.println("Mode 1 (auto): run");
  display.println("capture_screenshots.py on");
  display.println("connected PC - dumping");
  display.println("screenshots via Serial");
  display.println("port will start.");
  display.println();
  display.println("Mode 2 (manual): tilt the");
  display.println("side button up/down to");
  display.println("pick a font, press the");
  display.println("middle button to view it.");
  display.display();
}

static void showDoneScreen()
{
  display.setEpdMode(epd_mode_t::epd_fast);
  display.fillScreen(TFT_WHITE);
  display.setTextColor(TFT_BLACK);
  display.setFont(&fonts::DejaVu18);
  display.setTextSize(1, 1);
  display.setCursor(0, 0);
  display.println("Done.");
  display.println("All fonts captured.");
  display.println("Reset to run again.");
  display.display();

  Serial.flush();
  delay(200);
  M5.Power.powerOff();
}

static void runCapture()
{
  display.setEpdMode(epd_mode_t::epd_fast);
  for (size_t i = 0; i < kFontCount; ++i)
  {
    captureFont(kBuiltinFonts[i], i);
  }
  Serial.println();
  Serial.println("ALL_DONE");
  showDoneScreen();
}

#pragma once

#include <M5Unified.h>
#include <M5GFX.h>

extern M5GFX display;

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
  display.println("Top button cancels scan.");
  display.display();
}


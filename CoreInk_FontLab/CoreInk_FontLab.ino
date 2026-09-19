#include <M5Unified.h>
#include <M5GFX.h>

#include "CoreInk_FontLab.h"

M5GFX display;

void setup()
{
  Serial.begin(115200);
  M5.begin();

  display.begin();
  showInstructions();
}

void loop()
{
  M5.update();

  if (!inBrowseMode)
  {
    if (checkScriptReady())
    {
      runCapture();
    }
    else if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed())
    {
      enterBrowseMode();
    }
  }
  else if (!inContentView)
  {
    if (M5.BtnA.wasPressed())
    {
      selectNextFont();
    }
    else if (M5.BtnC.wasPressed())
    {
      selectPrevFont();
    }
    else if (M5.BtnB.wasPressed())
    {
      openSelectedFont();
    }
  }
  else
  {
    if (M5.BtnA.wasPressed())
    {
      contentNextPage();
    }
    else if (M5.BtnC.wasPressed())
    {
      contentPrevPage();
    }
    else if (M5.BtnB.wasPressed())
    {
      closeContentView();
    }
  }

  delay(20);
}

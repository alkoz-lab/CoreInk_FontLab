#pragma once

#include <M5Unified.h>
#include <M5GFX.h>
#include <algorithm>

#include "font_layout.h"

class FontMenu
{
public:
  FontMenu(M5GFX &display, const BuiltinFont *items, size_t itemCount)
      : display(display), items(items), itemCount(itemCount)
  {
  }

  void reset()
  {
    selected = 0;
    offset = 0;
  }

  void next()
  {
    const int rows = visibleRows();
    const int lastOffset = std::max(0, (int)itemCount - rows);
    if (selected == offset + rows - 1)
      offset = std::min(offset + rows, lastOffset);

    selected = itemCount == 0 ? 0 : (selected + 1) % itemCount;
  }

  void previous()
  {
    const int rows = visibleRows();
    const int lastOffset = std::max(0, (int)itemCount - rows);
    if (selected == offset)
      offset = offset >= rows ? offset - rows : lastOffset;

    selected = itemCount == 0 ? 0 : (selected + itemCount - 1) % itemCount;
  }

  size_t selectedIndex() const
  {
    return selected;
  }

  void render()
  {
    const int rows = visibleRows();
    const int start = offset;
    const int end = std::min(start + rows, (int)itemCount);
    const int rowHeight = menuFontHeight();

    display.setFont(&fonts::AsciiFont8x16);
    display.setTextSize(1, 1);
    display.setTextColor(TFT_BLACK);
    display.setTextDatum(textdatum_t::top_left);
    display.setTextWrap(false, false);
    display.fillScreen(TFT_WHITE);

    for (int i = start; i < end; ++i)
    {
      const int y = menuTop + (i - start) * rowHeight;
      if (i == (int)selected)
      {
        display.fillRect(0, y, display.width(), rowHeight - 2, TFT_BLACK);
        display.setTextColor(TFT_WHITE);
      }
      else
      {
        display.setTextColor(TFT_BLACK);
      }

      display.setCursor(0, y);
      display.printf("%2d %s", i + 1, items[i].name);
    }

    display.setFont(&fonts::Font8x8C64);
    display.setTextColor(TFT_BLACK);
    display.setCursor(0, display.height() - footerHeight);
    if (offset > 0)
    {
      display.write((uint8_t)0x8c);
      display.print(" MORE");
    }
    if (end < (int)itemCount)
    {
      display.print("   ");
      display.write((uint8_t)0x8F);
      display.print(" MORE");
    }
    display.print("   ");
    display.write((uint8_t)0x8A);
    display.print(" VIEW");
  }

  int visibleRows() const
  {
    return std::max(1, static_cast<int>((display.height() - menuTop - footerHeight) / menuFontHeight()));
  }

private:
  int menuFontHeight() const
  {
    display.setFont(&fonts::AsciiFont8x16);
    display.setTextSize(1, 1);
    return static_cast<int>(display.fontHeight());
  }

  M5GFX &display;
  const BuiltinFont *items;
  size_t itemCount;
  size_t selected = 0;
  int offset = 0;

  static const int menuTop = 4;
  static const int footerHeight = 12;
};

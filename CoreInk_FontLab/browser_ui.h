#pragma once

#include <M5Unified.h>
#include <M5GFX.h>
#include <algorithm>
#include <cstdio>
#include <vector>

#include "font_layout.h"
#include "font_menu.h"

extern M5GFX display;

static bool inBrowseMode = false;
static bool inContentView = false;
static int contentPageIndex = 0;
static std::vector<Glyph> browseGlyphs;
static std::vector<int> browsePageStarts;
static int browseTotalPages = 0;
static int browseFontH = 0;
static int browseAscent = 0;
static int browseSpecialPageCount = 0;
static FontMenu fontMenu(display, kBuiltinFonts, kFontCount);

static bool isSpecialCharacterFont(const BuiltinFont &bf)
{
  static const char *kSpecialFonts[] = {
      "AsciiFont8x16",
      "Font0_GLCD6x8",
      "Font8x8C64",
      "Font2",
  };

  for (const char *fontName : kSpecialFonts)
  {
    if (strcmp(bf.name, fontName) == 0)
      return true;
  }
  return false;
}

static bool isExcludedFromSpecialTable(uint16_t code)
{
  if (code >= '0' && code <= '9')
    return true;
  if (code >= 'A' && code <= 'Z')
    return true;
  if (code >= 'a' && code <= 'z')
    return true;
  return false;
}

static std::vector<Glyph> buildSpecialGlyphs(const std::vector<Glyph> &glyphs)
{
  std::vector<Glyph> filtered;
  filtered.reserve(glyphs.size());
  for (const Glyph &g : glyphs)
  {
    if (!isExcludedFromSpecialTable(g.code))
      filtered.push_back(g);
  }
  return filtered;
}

struct SpecialCharacterLayout
{
  int rowsPerColumn;
  int columns;
  int labelWidth;
  int glyphWidth;
  int columnGap;
};

static SpecialCharacterLayout getSpecialCharacterLayout(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int fontH)
{
  const int leftMargin = 4;
  const int topMargin = 18;
  const int glyphGap = 3;

  display.setFont(bf.font);
  display.setTextSize(1, 1);

  int labelWidth = 0;
  for (int code = 0; code <= 0xFF; ++code)
  {
    char label[3];
    snprintf(label, sizeof(label), "%02X", code);
    labelWidth = std::max(labelWidth, static_cast<int>(display.textWidth(label)));
  }

  int glyphWidth = 0;
  for (const Glyph &g : glyphs)
    glyphWidth = std::max(glyphWidth, (int)g.width);

  const int columnGap = std::max(1, static_cast<int>(display.textWidth("   ")));
  const int cellWidth = labelWidth + glyphGap + glyphWidth;
  const int rowHeight = fontH + 2;
  const int rowsPerColumn = std::max(1, static_cast<int>((display.height() - topMargin) / rowHeight));
  const int columns = std::max(1, static_cast<int>((display.width() - leftMargin + columnGap) / (cellWidth + columnGap)));

  return {rowsPerColumn, columns, labelWidth, glyphWidth, columnGap};
}

static int getSpecialCharacterPageCount(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int fontH)
{
  const SpecialCharacterLayout layout = getSpecialCharacterLayout(bf, glyphs, fontH);
  const int cellsPerPage = layout.rowsPerColumn * layout.columns;
  return (int)((glyphs.size() + cellsPerPage - 1) / cellsPerPage);
}

static void renderSpecialCharacterPage(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int pageIndex, int fontH)
{
  const SpecialCharacterLayout layout = getSpecialCharacterLayout(bf, glyphs, fontH);
  const int rowsPerColumn = layout.rowsPerColumn;
  const int columns = layout.columns;
  const int cellsPerPage = rowsPerColumn * columns;
  const int startIndex = pageIndex * cellsPerPage;
  const int endIndex = startIndex + cellsPerPage;

  display.fillScreen(TFT_WHITE);

  display.setFont(&fonts::AsciiFont8x16);
  display.setTextSize(1, 1);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(false, false);
  display.setCursor(0, 0);
  display.printf("%s %d/%d", bf.name, pageIndex + 1, getSpecialCharacterPageCount(bf, glyphs, fontH));

  display.setFont(bf.font);
  display.setTextSize(1, 1);

  for (int row = 0; row < rowsPerColumn; ++row)
  {
    for (int col = 0; col < columns; ++col)
    {
      const int glyphIndex = row + col * rowsPerColumn + startIndex;
      if (glyphIndex < startIndex || glyphIndex >= endIndex || glyphIndex >= (int)glyphs.size())
        continue;

      const Glyph &g = glyphs[glyphIndex];
      const int x = 4 + col * (layout.labelWidth + layout.glyphWidth + 3 + layout.columnGap);
      const int y = 18 + row * (fontH + 2);

      display.setCursor(x, y);
      display.printf("%02X", (unsigned)g.code);
      display.drawChar(g.code, x + layout.labelWidth + 3, y);
    }
  }
}

static void ensureBrowseFont(int fontIndex)
{
  if (fontIndex < 0)
    fontIndex = 0;
  else if (fontIndex >= (int)kFontCount)
    fontIndex = (int)kFontCount - 1;

  const BuiltinFont &bf = kBuiltinFonts[fontIndex];
  display.setFont(bf.font);
  display.setTextSize(1, 1);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(false, false);

  browseFontH = display.fontHeight();
  browseAscent = getFontAscent(bf.font);
  browseGlyphs = detectPrintableGlyphs(bf.font, browseFontH);
  browsePageStarts = buildPageStarts(browseGlyphs, browseFontH, display.width(), display.height(), !bf.digitsOnly);

  const std::vector<Glyph> specialGlyphs = buildSpecialGlyphs(browseGlyphs);
  browseSpecialPageCount = isSpecialCharacterFont(bf) ? getSpecialCharacterPageCount(bf, specialGlyphs, browseFontH) : 0;
  browseTotalPages = (int)browsePageStarts.size() - 1 + browseSpecialPageCount;

}

static void showFontListPage()
{
  display.setEpdMode(epd_mode_t::epd_fast);
  fontMenu.render();
  display.display();
}

static void showContentPage()
{
  display.setEpdMode(epd_mode_t::epd_fast);
  const int selectedIndex = (int)fontMenu.selectedIndex();
  const BuiltinFont &bf = kBuiltinFonts[selectedIndex];

  int regularPageCount = (int)browsePageStarts.size() - 1;
  if (contentPageIndex < regularPageCount)
  {
    display.setFont(bf.font);
    display.setTextSize(1, 1);
    display.setTextColor(TFT_BLACK);
    display.setTextDatum(textdatum_t::top_left);
    display.setTextWrap(false, false);
    renderContentPage(bf, browseGlyphs, browsePageStarts[contentPageIndex], browsePageStarts[contentPageIndex + 1], contentPageIndex, browseFontH, display.width(), browseAscent);
    display.display();
    return;
  }

  if (isSpecialCharacterFont(bf))
  {
    const std::vector<Glyph> specialGlyphs = buildSpecialGlyphs(browseGlyphs);
    const int specialPageIndex = contentPageIndex - regularPageCount;
    if (specialPageIndex >= 0 && specialPageIndex < (int)specialGlyphs.size())
    {
      renderSpecialCharacterPage(bf, specialGlyphs, specialPageIndex, browseFontH);
      display.display();
      return;
    }
  }
}

static void enterBrowseMode()
{
  inBrowseMode = true;
  inContentView = false;
  display.setAutoDisplay(false);
  fontMenu.reset();
  ensureBrowseFont(0);
  showFontListPage();
}

static void selectNextFont()
{
  fontMenu.next();
  ensureBrowseFont((int)fontMenu.selectedIndex());
  showFontListPage();
}

static void selectPrevFont()
{
  fontMenu.previous();
  ensureBrowseFont((int)fontMenu.selectedIndex());
  showFontListPage();
}

static void openSelectedFont()
{
  inContentView = true;
  contentPageIndex = 0;
  ensureBrowseFont((int)fontMenu.selectedIndex());
  showContentPage();
}

static void contentNextPage()
{
  if (browseTotalPages <= 0)
    return;
  contentPageIndex = (contentPageIndex + 1) % browseTotalPages;
  showContentPage();
}

static void contentPrevPage()
{
  if (browseTotalPages <= 0)
    return;
  contentPageIndex = (contentPageIndex + browseTotalPages - 1) % browseTotalPages;
  showContentPage();
}

static void closeContentView()
{
  inContentView = false;
  showFontListPage();
}

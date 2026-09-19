#pragma once

#include <M5Unified.h>
#include <M5GFX.h>
#include <vector>

#include "font_layout.h"

extern M5GFX display;

static bool inBrowseMode = false;
static bool inContentView = false;
static int browseFontIndex = -1;
static int contentPageIndex = 0;
static std::vector<Glyph> browseGlyphs;
static std::vector<int> browsePageStarts;
static int browseTotalPages = 0;
static int browseFontH = 0;
static int browseAscent = 0;

static void renderFontInfoPage(size_t fontIndex, const BuiltinFont &bf, size_t glyphCount)
{
  display.setFont(&fonts::FreeMonoBold9pt7b);
  display.setTextSize(1, 1);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(true, false);
  display.fillScreen(TFT_WHITE);
  display.setCursor(0, 0);
  display.printf("Font %u/%u\n", (unsigned)(fontIndex + 1), (unsigned)kFontCount);
  display.println(bf.name);
  display.printf("%u glyphs\n", (unsigned)glyphCount);
  display.println();
  display.println("Up/down: navigate");
  display.println();
  display.println("Middle: view font");
  display.setTextWrap(false, false);
}

static void ensureBrowseFont(int fontIndex)
{
  if (browseFontIndex == fontIndex)
    return;
  const BuiltinFont &bf = kBuiltinFonts[fontIndex];
  display.setFont(bf.font);
  display.setTextSize(1, 1);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(false, false);
  browseFontH = display.fontHeight();
  browseAscent = getFontAscent(bf.font);
  browseGlyphs = detectPrintableGlyphs(bf.font, browseFontH);
  browsePageStarts = buildPageStarts(browseGlyphs, browseFontH, display.width(), display.height(), !bf.digitsOnly);
  browseTotalPages = (int)browsePageStarts.size() - 1;
  browseFontIndex = fontIndex;
}

static void showFontListPage()
{
  display.setEpdMode(epd_mode_t::epd_fast);
  renderFontInfoPage(browseFontIndex, kBuiltinFonts[browseFontIndex], browseGlyphs.size());
  display.display();
}

static void showContentPage()
{
  display.setEpdMode(epd_mode_t::epd_fast);
  const BuiltinFont &bf = kBuiltinFonts[browseFontIndex];
  display.setFont(bf.font);
  display.setTextSize(1, 1);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(false, false);
  renderContentPage(bf, browseGlyphs, browsePageStarts[contentPageIndex], browsePageStarts[contentPageIndex + 1], contentPageIndex, browseFontH, display.width(), browseAscent);
  display.display();
}

static void enterBrowseMode()
{
  inBrowseMode = true;
  inContentView = false;
  display.setAutoDisplay(false);
  ensureBrowseFont(0);
  showFontListPage();
}

static void selectNextFont()
{
  ensureBrowseFont((browseFontIndex + 1) % kFontCount);
  showFontListPage();
}

static void selectPrevFont()
{
  ensureBrowseFont((browseFontIndex + kFontCount - 1) % kFontCount);
  showFontListPage();
}

static void openSelectedFont()
{
  inContentView = true;
  contentPageIndex = 0;
  showContentPage();
}

static void contentNextPage()
{
  contentPageIndex = (contentPageIndex + 1) % browseTotalPages;
  showContentPage();
}

static void contentPrevPage()
{
  contentPageIndex = (contentPageIndex + browseTotalPages - 1) % browseTotalPages;
  showContentPage();
}

static void closeContentView()
{
  inContentView = false;
  showFontListPage();
}

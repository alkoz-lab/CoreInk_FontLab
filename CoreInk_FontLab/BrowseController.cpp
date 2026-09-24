#include "BrowseController.h"

#include <M5Unified.h>
#include <M5GFX.h>
#include <vector>

#include "FontRegistry.h"
#include "Glyph.h"
#include "GlyphScanner.h"
#include "PageLayout.h"
#include "PageRenderer.h"
#include "font_menu.h"

extern M5GFX display;

enum class ViewState
{
  FontList,
  ContentPage
};

static bool browseModeActive = false;
static ViewState viewState = ViewState::FontList;
static int contentPageIndex = 0;
static std::vector<Glyph> browseGlyphs;
static std::vector<int> browsePageStarts;
static int browseTotalPages = 0;
static int browseFontH = 0;
static int browseAscent = 0;
static int browseSpecialPageCount = 0;
static FontMenu fontMenu(display, kBuiltinFonts, kFontCount);

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
  browseGlyphs = detectPrintableGlyphs(bf, browseFontH, true);
  if (scanCancelled)
    return;
  browsePageStarts = buildPageStarts(browseGlyphs, browseFontH, display.width(), display.height(), !bf.digitsOnly);

  const std::vector<Glyph> specialGlyphs = buildSpecialGlyphs(browseGlyphs);
  const std::vector<Glyph> unicodeGlyphs = buildUnicodeGlyphs(browseGlyphs);
  browseSpecialPageCount = isUnicodeFont(bf)
                               ? getUnicodeCharacterPageCount(bf, unicodeGlyphs, browseFontH)
                               : (isSpecialCharacterFont(bf) ? getSpecialCharacterPageCount(bf, specialGlyphs, browseFontH) : 0);
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

  if (isUnicodeFont(bf))
  {
    const std::vector<Glyph> unicodeGlyphs = buildUnicodeGlyphs(browseGlyphs);
    const int unicodePageIndex = contentPageIndex - regularPageCount;
    if (unicodePageIndex >= 0 && unicodePageIndex < (int)browseSpecialPageCount)
    {
      renderUnicodeCharacterPage(bf, unicodeGlyphs, unicodePageIndex, browseFontH);
      display.display();
      return;
    }
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

static void showScanningPage()
{
  const BuiltinFont &bf = kBuiltinFonts[fontMenu.selectedIndex()];
  display.setEpdMode(epd_mode_t::epd_fast);
  display.fillScreen(TFT_WHITE);
  display.setFont(&fonts::AsciiFont8x16);
  display.setTextSize(1, 1);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(false, false);
  display.setCursor(0, 8);
  display.print("Scanning ");
  display.print(bf.name);
  display.setCursor(0, 32);
  display.print("Top button cancels");
  display.display();
}

bool isBrowseModeActive()
{
  return browseModeActive;
}

bool isContentViewActive()
{
  return viewState == ViewState::ContentPage;
}

void enterBrowseMode()
{
  browseModeActive = true;
  viewState = ViewState::FontList;
  display.setAutoDisplay(false);
  fontMenu.reset();
  showFontListPage();
}

void selectNextFont()
{
  fontMenu.next();
  showFontListPage();
}

void selectPrevFont()
{
  fontMenu.previous();
  showFontListPage();
}

void openSelectedFont()
{
  viewState = ViewState::ContentPage;
  contentPageIndex = 0;
  if (isUnicodeFont(kBuiltinFonts[fontMenu.selectedIndex()]))
    showScanningPage();
  ensureBrowseFont((int)fontMenu.selectedIndex());
  if (scanCancelled)
  {
    viewState = ViewState::FontList;
    showFontListPage();
    return;
  }
  showContentPage();
}

void contentNextPage()
{
  if (browseTotalPages <= 0)
    return;
  contentPageIndex = (contentPageIndex + 1) % browseTotalPages;
  showContentPage();
}

void contentPrevPage()
{
  if (browseTotalPages <= 0)
    return;
  contentPageIndex = (contentPageIndex + browseTotalPages - 1) % browseTotalPages;
  showContentPage();
}

void closeContentView()
{
  viewState = ViewState::FontList;
  showFontListPage();
}

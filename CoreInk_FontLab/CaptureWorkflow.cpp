#include "CaptureWorkflow.h"

#include <M5Unified.h>
#include <M5GFX.h>
#include <vector>

#include "Glyph.h"
#include "GlyphScanner.h"
#include "PageLayout.h"
#include "PageRenderer.h"
#include "PcLink.h"

extern M5GFX display;

// Terminal screen for the capture workflow; powers off the device once shown.
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

void captureFont(const BuiltinFont &bf, size_t fontIndex)
{
  display.setAutoDisplay(false);
  display.setFont(bf.font);
  display.setTextSize(1, 1);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(false, false);

  const int fontH = display.fontHeight();
  const int screenW = display.width();
  const int screenH = display.height();
  const int ascent = getFontAscent(bf.font);

  std::vector<Glyph> glyphs = detectPrintableGlyphs(bf, fontH, false);
  display.setFont(bf.font);
  display.setTextSize(1, 1);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(false, false);
  std::vector<int> pageStarts = buildPageStarts(glyphs, fontH, screenW, screenH, !bf.digitsOnly);
  const int regularPageCount = (int)pageStarts.size() - 1;
  const std::vector<Glyph> specialGlyphs = buildSpecialGlyphs(glyphs);
  const std::vector<Glyph> unicodeGlyphs = buildUnicodeGlyphs(glyphs);
  const int specialPageCount = isUnicodeFont(bf)
                                   ? getUnicodeCharacterPageCount(bf, unicodeGlyphs, fontH)
                                   : (isSpecialCharacterFont(bf) ? getSpecialCharacterPageCount(bf, specialGlyphs, fontH) : 0);
  const int totalPages = regularPageCount + specialPageCount;

  Serial.printf("%s detected codepoints (%u):", bf.name, (unsigned)glyphs.size());
  for (auto &g : glyphs)
  {
    Serial.printf(" %02X", g.code);
  }
  Serial.println();

  auto pageFileName = [&](int page)
  {
    if (totalPages == 1)
      return String(bf.name) + ".bmp";
    return String(bf.name) + "_page" + String(page) + ".bmp";
  };

  for (int p = 0; p < regularPageCount; ++p)
  {
    renderContentPage(bf, glyphs, pageStarts[p], pageStarts[p + 1], p, fontH, screenW, ascent);
    display.display();
    sendPageAndWaitAck(pageFileName(p + 1));
  }

  for (int p = 0; p < specialPageCount; ++p)
  {
    if (isUnicodeFont(bf))
      renderUnicodeCharacterPage(bf, unicodeGlyphs, p, fontH);
    else
      renderSpecialCharacterPage(bf, specialGlyphs, p, fontH);
    display.display();
    sendPageAndWaitAck(pageFileName(regularPageCount + p + 1));
  }

  Serial.printf("Font %u/%u done: %s (%d page%s, %u glyphs)\n",
                (unsigned)(fontIndex + 1), (unsigned)kFontCount, bf.name,
                totalPages, totalPages == 1 ? "" : "s", (unsigned)glyphs.size());
}

void runCapture()
{
  display.setEpdMode(epd_mode_t::epd_fast);
  for (size_t i = 0; i < kFontCount; ++i)
  {
    captureFont(kBuiltinFonts[i], i);
  }
  Serial.println();
  Serial.println("ALL_DONE");
  Serial.flush();
  showDoneScreen();
}

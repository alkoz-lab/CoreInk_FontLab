#include "GlyphScanner.h"

#include <M5Unified.h>

extern M5GFX display;

bool scanCancelled = false;

int getFontAscent(const lgfx::IFont *font)
{
  auto type = font->getType();
  if (type == lgfx::IFont::ft_glcd || type == lgfx::IFont::ft_bmp || type == lgfx::IFont::ft_rle)
  {
    return 0;
  }
  lgfx::FontMetrics fm{};
  font->getDefaultMetric(&fm);
  return fm.baseline;
}

bool bufferHasBits(const uint8_t *buf, size_t len)
{
  for (size_t i = 0; i < len; ++i)
  {
    if (buf[i] != 0)
      return true;
  }
  return false;
}

void renderScanProgress(const BuiltinFont &bf, uint32_t codeValue, size_t glyphCount)
{
  const uint32_t total = (uint32_t)bf.lastCode - bf.firstCode + 1;
  const uint32_t completed = codeValue - bf.firstCode + 1;
  const int percent = (int)((completed * 100ULL) / total);
  const int barWidth = display.width() - 16;

  display.setEpdMode(epd_mode_t::epd_fast);
  display.setFont(&fonts::AsciiFont8x16);
  display.setTextSize(1, 1);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(false, false);
  display.fillScreen(TFT_WHITE);
  display.setCursor(0, 8);
  display.print("Scanning ");
  display.print(bf.name);
  display.setCursor(0, 32);
  display.printf("%d%%  code %04lX", percent, (unsigned long)codeValue);
  display.setCursor(0, 52);
  display.printf("glyphs %lu", (unsigned long)glyphCount);
  display.setCursor(0, 100);
  display.print("Top button: cancel");
  display.drawRect(8, 76, barWidth, 12, TFT_BLACK);
  display.fillRect(10, 78, (barWidth - 4) * percent / 100, 8, TFT_BLACK);
  display.display();
  Serial.printf("SCAN_PROGRESS:%s:%d:%04lX:%lu\n",
                bf.name, percent, (unsigned long)codeValue, (unsigned long)glyphCount);
}

std::vector<Glyph> detectPrintableGlyphs(const BuiltinFont &bf, int fontH, bool allowCancel)
{
  std::vector<Glyph> glyphs;
  scanCancelled = false;
  const bool showProgress = isUnicodeFont(bf);

  const lgfx::IFont *font = bf.font;
  const int ascent = getFontAscent(font);
  const int canvasW = fontH * 3;
  const int canvasH = fontH + 12;
  const size_t rowBytes = (canvasW + 7) / 8;
  const size_t bufLen = rowBytes * canvasH;

  M5Canvas canvas(&display);
  canvas.setColorDepth(1);
  canvas.createSprite(canvasW, canvasH);
  canvas.setFont(font);
  canvas.setTextSize(1, 1);
  canvas.setTextDatum(textdatum_t::top_left);
  canvas.setTextWrap(false, false);
  canvas.setTextColor(1, 0);

  for (uint32_t codeValue = bf.firstCode; codeValue <= bf.lastCode; ++codeValue)
  {
    if ((codeValue - bf.firstCode) % 32 == 0)
    {
      M5.update();
      if (allowCancel && M5.BtnEXT.wasPressed())
      {
        scanCancelled = true;
        break;
      }
    }

    if (showProgress && (codeValue == bf.firstCode || codeValue == bf.lastCode || ((codeValue - bf.firstCode) % 512 == 0)))
      renderScanProgress(bf, codeValue, glyphs.size());

    const uint16_t code = (uint16_t)codeValue;
    lgfx::FontMetrics metrics{};
    font->getDefaultMetric(&metrics);
    if (!font->updateFontMetric(&metrics, code))
      continue;
    int16_t w = metrics.x_advance > 0 ? metrics.x_advance : metrics.width;
    if (w <= 0)
      continue;

    canvas.fillSprite(0);
    canvas.drawChar(code, 0, ascent);
    if (bufferHasBits((const uint8_t *)canvas.getBuffer(), bufLen))
    {
      glyphs.push_back({code, w});
    }
  }

  if (showProgress && !scanCancelled)
    renderScanProgress(bf, bf.lastCode, glyphs.size());

  // Progress rendering uses the UI font; restore the scanned font for the caller.
  display.setFont(bf.font);
  display.setTextSize(1, 1);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(false, false);
  canvas.deleteSprite();
  return glyphs;
}

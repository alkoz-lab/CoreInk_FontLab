#pragma once

#include <M5Unified.h>
#include <M5GFX.h>
#include <vector>
#include <cstring>

extern M5GFX display;

// ------------------------------------------------------------
// Built-in fonts to scan, at their natural 1x scale only.
// ------------------------------------------------------------
struct BuiltinFont
{
  const char *name;
  const lgfx::IFont *font;
  bool digitsOnly;
};

static const BuiltinFont kBuiltinFonts[] = {
    {"AsciiFont8x16", &fonts::AsciiFont8x16, false},
    {"DejaVu9", &fonts::DejaVu9, false},
    {"DejaVu12", &fonts::DejaVu12, false},
    {"DejaVu18", &fonts::DejaVu18, false},
    {"DejaVu24", &fonts::DejaVu24, false},
    {"Font0_GLCD6x8", &fonts::Font0, false},
    {"Font2", &fonts::Font2, false},
    {"Font4", &fonts::Font4, false},
    {"Font6", &fonts::Font6, true},
    {"Font7_7seg", &fonts::Font7, true},
    {"Font8", &fonts::Font8, true},
    {"Font8x8C64", &fonts::Font8x8C64, false},
    {"FreeMono9pt", &fonts::FreeMono9pt7b, false},
    {"FreeMono12pt", &fonts::FreeMono12pt7b, false},
    {"FreeMono18pt", &fonts::FreeMono18pt7b, false},
    {"FreeMono24pt", &fonts::FreeMono24pt7b, false},
    {"FreeMonoBold9pt", &fonts::FreeMonoBold9pt7b, false},
    {"FreeMonoBold12pt", &fonts::FreeMonoBold12pt7b, false},
    {"FreeMonoBold18pt", &fonts::FreeMonoBold18pt7b, false},
    {"FreeMonoBold24pt", &fonts::FreeMonoBold24pt7b, false},
    {"FreeMonoBoldOblique9pt", &fonts::FreeMonoBoldOblique9pt7b, false},
    {"FreeMonoBoldOblique12pt", &fonts::FreeMonoBoldOblique12pt7b, false},
    {"FreeMonoBoldOblique18pt", &fonts::FreeMonoBoldOblique18pt7b, false},
    {"FreeMonoBoldOblique24pt", &fonts::FreeMonoBoldOblique24pt7b, false},
    {"FreeMonoOblique9pt", &fonts::FreeMonoOblique9pt7b, false},
    {"FreeMonoOblique12pt", &fonts::FreeMonoOblique12pt7b, false},
    {"FreeMonoOblique18pt", &fonts::FreeMonoOblique18pt7b, false},
    {"FreeMonoOblique24pt", &fonts::FreeMonoOblique24pt7b, false},
    {"FreeSans9pt", &fonts::FreeSans9pt7b, false},
    {"FreeSans12pt", &fonts::FreeSans12pt7b, false},
    {"FreeSans18pt", &fonts::FreeSans18pt7b, false},
    {"FreeSans24pt", &fonts::FreeSans24pt7b, false},
    {"FreeSansBold9pt", &fonts::FreeSansBold9pt7b, false},
    {"FreeSansBold12pt", &fonts::FreeSansBold12pt7b, false},
    {"FreeSansBold18pt", &fonts::FreeSansBold18pt7b, false},
    {"FreeSansBold24pt", &fonts::FreeSansBold24pt7b, false},
    {"FreeSansBoldOblique9pt", &fonts::FreeSansBoldOblique9pt7b, false},
    {"FreeSansBoldOblique12pt", &fonts::FreeSansBoldOblique12pt7b, false},
    {"FreeSansBoldOblique18pt", &fonts::FreeSansBoldOblique18pt7b, false},
    {"FreeSansBoldOblique24pt", &fonts::FreeSansBoldOblique24pt7b, false},
    {"FreeSansOblique9pt", &fonts::FreeSansOblique9pt7b, false},
    {"FreeSansOblique12pt", &fonts::FreeSansOblique12pt7b, false},
    {"FreeSansOblique18pt", &fonts::FreeSansOblique18pt7b, false},
    {"FreeSansOblique24pt", &fonts::FreeSansOblique24pt7b, false},
    {"FreeSerif9pt", &fonts::FreeSerif9pt7b, false},
    {"FreeSerif12pt", &fonts::FreeSerif12pt7b, false},
    {"FreeSerif18pt", &fonts::FreeSerif18pt7b, false},
    {"FreeSerif24pt", &fonts::FreeSerif24pt7b, false},
    {"FreeSerifBold9pt", &fonts::FreeSerifBold9pt7b, false},
    {"FreeSerifBold12pt", &fonts::FreeSerifBold12pt7b, false},
    {"FreeSerifBold18pt", &fonts::FreeSerifBold18pt7b, false},
    {"FreeSerifBold24pt", &fonts::FreeSerifBold24pt7b, false},
    {"FreeSerifBoldItalic9pt", &fonts::FreeSerifBoldItalic9pt7b, false},
    {"FreeSerifBoldItalic12pt", &fonts::FreeSerifBoldItalic12pt7b, false},
    {"FreeSerifBoldItalic18pt", &fonts::FreeSerifBoldItalic18pt7b, false},
    {"FreeSerifBoldItalic24pt", &fonts::FreeSerifBoldItalic24pt7b, false},
    {"FreeSerifItalic9pt", &fonts::FreeSerifItalic9pt7b, false},
    {"FreeSerifItalic12pt", &fonts::FreeSerifItalic12pt7b, false},
    {"FreeSerifItalic18pt", &fonts::FreeSerifItalic18pt7b, false},
    {"FreeSerifItalic24pt", &fonts::FreeSerifItalic24pt7b, false},
    {"Orbitron_Light_24", &fonts::Orbitron_Light_24, false},
    {"Orbitron_Light_32", &fonts::Orbitron_Light_32, false},
    {"Roboto_Thin_24", &fonts::Roboto_Thin_24, false},
    {"Satisfy_24", &fonts::Satisfy_24, false},
    {"TomThumb", &fonts::TomThumb, false},
    {"Yellowtail_32", &fonts::Yellowtail_32, false},
};
static const size_t kFontCount = sizeof(kBuiltinFonts) / sizeof(kBuiltinFonts[0]);

// Codepoint range scanned for printable glyphs (skips ASCII whitespace/control and DEL).
static const uint16_t kFirstCandidateCode = 33;
static const uint16_t kLastCandidateCode = 255;
static const uint32_t kAckTimeoutMs = 18000;

struct Glyph
{
  uint16_t code;
  int16_t width;
};

static void drawDigitsRow(int y, int ascent);

static bool bufferHasBits(const uint8_t *buf, size_t len)
{
  for (size_t i = 0; i < len; ++i)
  {
    if (buf[i] != 0)
      return true;
  }
  return false;
}

static int getFontAscent(const lgfx::IFont *font)
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

static std::vector<Glyph> detectPrintableGlyphs(const lgfx::IFont *font, int fontH)
{
  std::vector<Glyph> glyphs;

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

  for (uint16_t code = kFirstCandidateCode; code <= kLastCandidateCode; ++code)
  {
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

  canvas.deleteSprite();
  return glyphs;
}

static std::vector<int> buildPageStarts(const std::vector<Glyph> &glyphs, int fontH, int screenW, int screenH, bool hasDigitsHeader)
{
  std::vector<int> starts;
  starts.push_back(0);
  int x = 0;
  int y = hasDigitsHeader ? fontH : 0;
  for (size_t i = 0; i < glyphs.size(); ++i)
  {
    const Glyph &g = glyphs[i];
    if (x > 0 && x + g.width > screenW)
    {
      x = 0;
      y += fontH;
    }
    if (y + fontH > screenH)
    {
      starts.push_back((int)i);
      x = 0;
      y = 0;
    }
    x += g.width;
  }
  starts.push_back((int)glyphs.size());
  return starts;
}

static void renderContentPage(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int beginIdx, int endIdx, int pageIndex, int fontH, int screenW, int ascent)
{
  display.fillScreen(TFT_WHITE);
  int x = 0;
  int y;
  if (pageIndex == 0 && !bf.digitsOnly)
  {
    drawDigitsRow(0, ascent);
    y = fontH;
  }
  else
  {
    y = 0;
  }
  for (int i = beginIdx; i < endIdx; ++i)
  {
    const Glyph &g = glyphs[i];
    if (x > 0 && x + g.width > screenW)
    {
      x = 0;
      y += fontH;
    }
    display.drawChar(g.code, x, y + ascent);
    x += g.width;
  }
}

static void drawDigitsRow(int y, int ascent)
{
  static const char kDigits[] = "1234567890";
  int x = 0;
  int i = 0;
  while (true)
  {
    char c = kDigits[i % 10];
    int w = display.textWidth(String(c));
    if (x + w > display.width())
      break;
    display.drawChar((uint16_t)(uint8_t)c, x, y + ascent);
    x += w;
    i++;
  }
}

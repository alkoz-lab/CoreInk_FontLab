#include "PageRenderer.h"

#include "GlyphScanner.h"
#include "PageLayout.h"

extern M5GFX display;

void drawDigitsRow(int y, int ascent)
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

void renderContentPage(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int beginIdx, int endIdx, int pageIndex, int fontH, int screenW, int ascent)
{
  display.setFont(bf.font);
  display.setTextSize(1, 1);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(false, false);
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

void renderSpecialCharacterPage(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int pageIndex, int fontH)
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
  const int ascent = getFontAscent(bf.font);

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
      display.drawChar(g.code, x + layout.labelWidth + 3, y + ascent);
    }
  }
}

void renderUnicodeCharacterPage(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int pageIndex, int fontH)
{
  const UnicodeCharacterLayout layout = getUnicodeCharacterLayout(bf, glyphs, fontH);
  const int rows = layout.rows;
  const int columns = 3;
  const int cellsPerPage = rows * columns;
  const int startIndex = pageIndex * cellsPerPage;
  const int endIndex = startIndex + cellsPerPage;

  display.fillScreen(TFT_WHITE);
  display.setFont(bf.font);
  display.setTextSize(1, 1);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(textdatum_t::top_left);
  display.setTextWrap(false, false);
  display.setCursor(0, 0);
  display.printf("%s U+%d/%d", bf.name, pageIndex + 1, getUnicodeCharacterPageCount(bf, glyphs, fontH));

  for (int row = 0; row < rows; ++row)
  {
    for (int col = 0; col < columns; ++col)
    {
      const int glyphIndex = row + col * rows + startIndex;
      if (glyphIndex < startIndex || glyphIndex >= endIndex || glyphIndex >= (int)glyphs.size())
        continue;

      const Glyph &g = glyphs[glyphIndex];
      const int x = 4 + col * (layout.labelWidth + layout.glyphWidth + 2 + layout.columnGap);
      const int y = 18 + row * (fontH + 2);
      display.setCursor(x, y);
      display.printf("%04X", (unsigned)g.code);
    }
  }

  display.setFont(bf.font);
  display.setTextSize(1, 1);
  const int ascent = getFontAscent(bf.font);
  for (int row = 0; row < rows; ++row)
  {
    for (int col = 0; col < columns; ++col)
    {
      const int glyphIndex = row + col * rows + startIndex;
      if (glyphIndex < startIndex || glyphIndex >= endIndex || glyphIndex >= (int)glyphs.size())
        continue;

      const Glyph &g = glyphs[glyphIndex];
      const int x = 4 + col * (layout.labelWidth + layout.glyphWidth + 3 + layout.columnGap);
      const int y = 18 + row * (fontH + 2);
      display.drawChar(g.code, x + layout.labelWidth + 3, y + ascent);
    }
  }
}

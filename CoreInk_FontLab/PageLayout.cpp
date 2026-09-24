#include "PageLayout.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

extern M5GFX display;

std::vector<int> buildPageStarts(const std::vector<Glyph> &glyphs, int fontH, int screenW, int screenH, bool hasDigitsHeader)
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

bool isSpecialCharacterFont(const BuiltinFont &bf)
{
  static const char *kSpecialFonts[] = {
      "AsciiFont8x16",
      "Font0_GLCD6x8",
      "Font8x8C64",
      "Font2",
      "efontKR_16"
    };

  for (const char *fontName : kSpecialFonts)
  {
    if (strcmp(bf.name, fontName) == 0)
      return true;
  }
  return false;
}

bool isExcludedFromSpecialTable(uint16_t code)
{
  if (code >= '0' && code <= '9')
    return true;
  if (code >= 'A' && code <= 'Z')
    return true;
  if (code >= 'a' && code <= 'z')
    return true;
  return false;
}

std::vector<Glyph> buildSpecialGlyphs(const std::vector<Glyph> &glyphs)
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

std::vector<Glyph> buildUnicodeGlyphs(const std::vector<Glyph> &glyphs)
{
  std::vector<Glyph> filtered;
  filtered.reserve(glyphs.size());
  for (const Glyph &g : glyphs)
  {
    const bool inRangeForCharsIamInterested = g.code >= 0x00A1 && g.code <= 0x3015 || g.code >= 0x3380 && g.code <= 0x33DD
     || g.code >= 0xFF0A && g.code <= 0xFFE6;
    if (inRangeForCharsIamInterested)
      filtered.push_back(g);
  }
  return filtered;
}

SpecialCharacterLayout getSpecialCharacterLayout(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int fontH)
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

int getSpecialCharacterPageCount(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int fontH)
{
  const SpecialCharacterLayout layout = getSpecialCharacterLayout(bf, glyphs, fontH);
  const int cellsPerPage = layout.rowsPerColumn * layout.columns;
  return (int)((glyphs.size() + cellsPerPage - 1) / cellsPerPage);
}

UnicodeCharacterLayout getUnicodeCharacterLayout(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int fontH)
{
  const int topMargin = 18;

  display.setFont(bf.font);
  display.setTextSize(1, 1);

  const int labelWidth = display.textWidth("FFFF");
  int glyphWidth = 0;
  for (const Glyph &g : glyphs)
    glyphWidth = std::max(glyphWidth, (int)g.width);

  const int columnGap = std::max(1, static_cast<int>(display.textWidth("  ")));
  const int rows = std::max(1, static_cast<int>((display.height() - topMargin) / (fontH + 2)));
  return {rows, labelWidth, glyphWidth, columnGap};
}

int getUnicodeCharacterPageCount(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int fontH)
{
  const UnicodeCharacterLayout layout = getUnicodeCharacterLayout(bf, glyphs, fontH);
  const int columns = 3;
  const int cellsPerPage = layout.rows * columns;
  return (int)((glyphs.size() + cellsPerPage - 1) / cellsPerPage);
}

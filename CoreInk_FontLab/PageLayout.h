#pragma once

#include <M5GFX.h>
#include <vector>

#include "FontRegistry.h"
#include "Glyph.h"

// Regular running-text page pagination (used by capture + browse content pages).
std::vector<int> buildPageStarts(const std::vector<Glyph> &glyphs, int fontH, int screenW, int screenH, bool hasDigitsHeader);

// Special character table classification (hex-code + glyph grid for select fonts).
bool isSpecialCharacterFont(const BuiltinFont &bf);
bool isExcludedFromSpecialTable(uint16_t code);
std::vector<Glyph> buildSpecialGlyphs(const std::vector<Glyph> &glyphs);

struct SpecialCharacterLayout
{
  int rowsPerColumn;
  int columns;
  int labelWidth;
  int glyphWidth;
  int columnGap;
};

SpecialCharacterLayout getSpecialCharacterLayout(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int fontH);
int getSpecialCharacterPageCount(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int fontH);

// Unicode character table classification (U+XXXX + glyph grid for Unicode fonts).
std::vector<Glyph> buildUnicodeGlyphs(const std::vector<Glyph> &glyphs);

struct UnicodeCharacterLayout
{
  int rows;
  int labelWidth;
  int glyphWidth;
  int columnGap;
};

UnicodeCharacterLayout getUnicodeCharacterLayout(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int fontH);
int getUnicodeCharacterPageCount(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int fontH);

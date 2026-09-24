#pragma once

#include <M5GFX.h>
#include <vector>

#include "FontRegistry.h"
#include "Glyph.h"

void drawDigitsRow(int y, int ascent);
void renderContentPage(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int beginIdx, int endIdx, int pageIndex, int fontH, int screenW, int ascent);
void renderSpecialCharacterPage(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int pageIndex, int fontH);
void renderUnicodeCharacterPage(const BuiltinFont &bf, const std::vector<Glyph> &glyphs, int pageIndex, int fontH);

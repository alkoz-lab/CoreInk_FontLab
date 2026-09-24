#pragma once

#include <M5GFX.h>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "FontRegistry.h"
#include "Glyph.h"

// Set by detectPrintableGlyphs() when the user cancels a scan (allowCancel = true).
extern bool scanCancelled;

int getFontAscent(const lgfx::IFont *font);
bool bufferHasBits(const uint8_t *buf, size_t len);
void renderScanProgress(const BuiltinFont &bf, uint32_t codeValue, size_t glyphCount);
std::vector<Glyph> detectPrintableGlyphs(const BuiltinFont &bf, int fontH, bool allowCancel);

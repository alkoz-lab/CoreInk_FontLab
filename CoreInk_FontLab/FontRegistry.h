#pragma once

#include <M5GFX.h>
#include <cstddef>
#include <cstdint>

struct BuiltinFont
{
  const char *name;
  const lgfx::IFont *font;
  bool digitsOnly;
  uint16_t firstCode;
  uint16_t lastCode;
};

extern const BuiltinFont kBuiltinFonts[];
extern const size_t kFontCount;

bool isUnicodeFont(const BuiltinFont &bf);

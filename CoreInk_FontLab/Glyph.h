#pragma once

#include <cstdint>

// A single renderable glyph detected during a font scan.
struct Glyph
{
  uint16_t code;
  int16_t width;
};

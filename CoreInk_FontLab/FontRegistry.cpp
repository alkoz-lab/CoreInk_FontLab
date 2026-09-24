#include "FontRegistry.h"

#include <M5Unified.h>

#define FONT_ENTRY(label, symbol) {label, &fonts::symbol, false, 33, 255}
#define DIGIT_FONT_ENTRY(label, symbol) {label, &fonts::symbol, true, 33, 255}
#define UNICODE_FONT_ENTRY(label, symbol) {label, &fonts::symbol, false, 32, 0xFFFF}

const BuiltinFont kBuiltinFonts[] = {
    FONT_ENTRY("AsciiFont8x16", AsciiFont8x16),
    FONT_ENTRY("AsciiFont24x48", AsciiFont24x48),
    FONT_ENTRY("DejaVu9", DejaVu9),
    FONT_ENTRY("DejaVu12", DejaVu12),
    FONT_ENTRY("DejaVu18", DejaVu18),
    FONT_ENTRY("DejaVu24", DejaVu24),
    FONT_ENTRY("DejaVu40", DejaVu40),
    //FONT_ENTRY("DejaVu56", DejaVu56), // Too large for practical use
    //FONT_ENTRY("DejaVu72", DejaVu72), // Too large for practical use
    {"Font0_GLCD6x8", &fonts::Font0, false, 33, 255},
    {"Font2", &fonts::Font2, false, 33, 255},
    {"Font4", &fonts::Font4, false, 33, 255},
    DIGIT_FONT_ENTRY("Font6", Font6),
    DIGIT_FONT_ENTRY("Font7_7seg", Font7),
    DIGIT_FONT_ENTRY("Font8", Font8),
    FONT_ENTRY("Font8x8C64", Font8x8C64),
    FONT_ENTRY("FreeMono9pt", FreeMono9pt7b),
    FONT_ENTRY("FreeMono12pt", FreeMono12pt7b),
    FONT_ENTRY("FreeMono18pt", FreeMono18pt7b),
    FONT_ENTRY("FreeMono24pt", FreeMono24pt7b),
    FONT_ENTRY("FreeMonoBold9pt", FreeMonoBold9pt7b),
    FONT_ENTRY("FreeMonoBold12pt", FreeMonoBold12pt7b),
    FONT_ENTRY("FreeMonoBold18pt", FreeMonoBold18pt7b),
    FONT_ENTRY("FreeMonoBold24pt", FreeMonoBold24pt7b),
    FONT_ENTRY("FreeMonoBoldOblique9pt", FreeMonoBoldOblique9pt7b),
    FONT_ENTRY("FreeMonoBoldOblique12pt", FreeMonoBoldOblique12pt7b),
    FONT_ENTRY("FreeMonoBoldOblique18pt", FreeMonoBoldOblique18pt7b),
    FONT_ENTRY("FreeMonoBoldOblique24pt", FreeMonoBoldOblique24pt7b),
    FONT_ENTRY("FreeMonoOblique9pt", FreeMonoOblique9pt7b),
    FONT_ENTRY("FreeMonoOblique12pt", FreeMonoOblique12pt7b),
    FONT_ENTRY("FreeMonoOblique18pt", FreeMonoOblique18pt7b),
    FONT_ENTRY("FreeMonoOblique24pt", FreeMonoOblique24pt7b),
    FONT_ENTRY("FreeSans9pt", FreeSans9pt7b),
    FONT_ENTRY("FreeSans12pt", FreeSans12pt7b),
    FONT_ENTRY("FreeSans18pt", FreeSans18pt7b),
    FONT_ENTRY("FreeSans24pt", FreeSans24pt7b),
    FONT_ENTRY("FreeSansBold9pt", FreeSansBold9pt7b),
    FONT_ENTRY("FreeSansBold12pt", FreeSansBold12pt7b),
    FONT_ENTRY("FreeSansBold18pt", FreeSansBold18pt7b),
    FONT_ENTRY("FreeSansBold24pt", FreeSansBold24pt7b),
    FONT_ENTRY("FreeSansBoldOblique9pt", FreeSansBoldOblique9pt7b),
    FONT_ENTRY("FreeSansBoldOblique12pt", FreeSansBoldOblique12pt7b),
    FONT_ENTRY("FreeSansBoldOblique18pt", FreeSansBoldOblique18pt7b),
    FONT_ENTRY("FreeSansBoldOblique24pt", FreeSansBoldOblique24pt7b),
    FONT_ENTRY("FreeSansOblique9pt", FreeSansOblique9pt7b),
    FONT_ENTRY("FreeSansOblique12pt", FreeSansOblique12pt7b),
    FONT_ENTRY("FreeSansOblique18pt", FreeSansOblique18pt7b),
    FONT_ENTRY("FreeSansOblique24pt", FreeSansOblique24pt7b),
    FONT_ENTRY("FreeSerif9pt", FreeSerif9pt7b),
    FONT_ENTRY("FreeSerif12pt", FreeSerif12pt7b),
    FONT_ENTRY("FreeSerif18pt", FreeSerif18pt7b),
    FONT_ENTRY("FreeSerif24pt", FreeSerif24pt7b),
    FONT_ENTRY("FreeSerifBold9pt", FreeSerifBold9pt7b),
    FONT_ENTRY("FreeSerifBold12pt", FreeSerifBold12pt7b),
    FONT_ENTRY("FreeSerifBold18pt", FreeSerifBold18pt7b),
    FONT_ENTRY("FreeSerifBold24pt", FreeSerifBold24pt7b),
    FONT_ENTRY("FreeSerifBoldItalic9pt", FreeSerifBoldItalic9pt7b),
    FONT_ENTRY("FreeSerifBoldItalic12pt", FreeSerifBoldItalic12pt7b),
    FONT_ENTRY("FreeSerifBoldItalic18pt", FreeSerifBoldItalic18pt7b),
    FONT_ENTRY("FreeSerifBoldItalic24pt", FreeSerifBoldItalic24pt7b),
    FONT_ENTRY("FreeSerifItalic9pt", FreeSerifItalic9pt7b),
    FONT_ENTRY("FreeSerifItalic12pt", FreeSerifItalic12pt7b),
    FONT_ENTRY("FreeSerifItalic18pt", FreeSerifItalic18pt7b),
    FONT_ENTRY("FreeSerifItalic24pt", FreeSerifItalic24pt7b),
    FONT_ENTRY("Orbitron_Light_24", Orbitron_Light_24),
    FONT_ENTRY("Orbitron_Light_32", Orbitron_Light_32),
    FONT_ENTRY("Roboto_Thin_24", Roboto_Thin_24),
    FONT_ENTRY("Satisfy_24", Satisfy_24),
    FONT_ENTRY("TomThumb", TomThumb),
    FONT_ENTRY("Yellowtail_32", Yellowtail_32),

    // The complete Asian catalog exceeds the CoreInk flash budget.
    // Uncomment one at a time
    //UNICODE_FONT_ENTRY("efontCN_12", efontCN_12),
    UNICODE_FONT_ENTRY("efontKR_16", efontKR_16),
    //UNICODE_FONT_ENTRY("efontJA_14", efontJA_14),
    //UNICODE_FONT_ENTRY("efontTW_16", efontTW_16),
    //UNICODE_FONT_ENTRY("lgfxJapanMincho_8", lgfxJapanMincho_8)
    //UNICODE_FONT_ENTRY("lgfxJapanMinchoP_12", lgfxJapanMinchoP_12)
    //UNICODE_FONT_ENTRY("lgfxJapanGothic_14", lgfxJapanGothic_14)
    //UNICODE_FONT_ENTRY("lgfxJapanGothicP_16", lgfxJapanGothicP_16)
};
#undef EFONT_FAMILY_ENTRIES
#undef EFONT_FAMILY_ENTRY
#undef JAPAN_FONT_ENTRY
#undef UNICODE_FONT_ENTRY
#undef DIGIT_FONT_ENTRY
#undef FONT_ENTRY

const size_t kFontCount = sizeof(kBuiltinFonts) / sizeof(kBuiltinFonts[0]);

bool isUnicodeFont(const BuiltinFont &bf)
{
  return bf.lastCode > 255;
}

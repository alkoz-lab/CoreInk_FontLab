#pragma once

// Whether Mode 2 (manual browse) is active, vs. Mode 1 auto-capture.
bool isBrowseModeActive();
// Whether the font list or a font's content/character-table page is shown.
bool isContentViewActive();

void enterBrowseMode();
void selectNextFont();
void selectPrevFont();
void openSelectedFont();
void contentNextPage();
void contentPrevPage();
void closeContentView();

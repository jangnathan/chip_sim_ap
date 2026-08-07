#pragma once
#include "ui.h"

typedef struct {
  UICachedText fileText;
  u8 fileSectionActiveDelay;

  u8 fileButtonHovered;

  UICachedText newText;
} Menubar;

void initMenubar(Menubar *menubar, UICtx *ctx);
void menubarUI(Menubar *menubar, UICtx *ctx);
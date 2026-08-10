#pragma once
#include "ui.h"

typedef struct {
  u8 fileSectionActiveDelay;

  u8 fileButtonHovered;

  UICachedText fileText;
  UICachedText newText;
  UICachedText saveFileText;
  UICachedText openFileText;

  u16 hoverID;
} Menubar;

void initMenubar(Menubar *menubar, UICtx *ctx);
void menubarUI(Menubar *menubar, UICtx *ctx);
#pragma once
#include "ui.h"
#include "core/window.h"
#include <SDL3/SDL.h>

typedef struct {
  tWindow *window;

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
#pragma once
#include "editor/editor.h"
#include "ui.h"

typedef struct {
  UICachedText startSimulationText;
  UICachedText stopSimulationText;

  UICachedText pivotText;
  UICachedText wireText;

  UICachedText switchText;

  UICachedText simpleChipsText[SIMPLE_CHIP_TYPE_END];
  UICachedText andGateText;

  UICachedText deleteText;

  u16 menubarHeight;
} EditorUI;

void editorUI_init(UICtx *ctx, EditorUI *editorUI);
void editorUI_run(UICtx *ctx, Editor *editor, EditorUI *editorUI);
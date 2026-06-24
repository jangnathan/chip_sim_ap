#include "editor.h"
#include "simulation/simulate.h"

#include "app.h"

#include <stdio.h>

void editorZoomOut(Editor *editor) {
  editor->camera.zoom += 0.1f;

  if (editor->camera.zoom >= 3.0f) {
    editor->camera.zoom = 3.0f;
  }
}
void editorZoomIn(Editor *editor) {
  editor->camera.zoom -= 0.1f;

  if (editor->camera.zoom <= 0.1f) {
    editor->camera.zoom = 0.1f;
  }
}

void editorHandleKeypress(Editor *editor, SDL_KeyboardEvent event) {
  SDL_Scancode code = event.scancode;
  if (code == editor->zoomInKey) {
    editorZoomIn(editor);
  } else if (code == editor->zoomOutKey) {
    editorZoomOut(editor);
  }
}

void updateEditor(Editor *editor, Input *input) {
  Circuit *circuit = &editor->ctx->circuit;

  const bool *keystates = SDL_GetKeyboardState(NULL);
  editor->selectBoxActive = 0;

  if (keystates[SDL_SCANCODE_SPACE]) {
    input->mouse.cursorIcon = CURSOR_MOVE;

    if (input->mouse.leftDown) {
      editor->camera.oldPosition = editor->camera.position;
    }

    if (input->mouse.leftHeld) {
      editor->camera.position = translateVec2f(
          editor->camera.oldPosition,
          scaleVec2f(vec2ItoF(subtractVec2i(input->mouse.oldCenterPosition,
                                            input->mouse.centerPosition)),
                     1.0f / editor->camera.zoom));
    }
    return;
  }

  switch (editor->state) {
  case EDIT_MOVE_CE: {
    Vec2f mousePos = vec2ItoF(input->mouse.centerPosition);
    // mousePos.y += editor->menubarHeight / 2;

    // positioning logic
    Vec2f pos1 = translateVec2f(editor->camera.position, mousePos);
    Vec2f pos = scaleVec2f(pos1, 1.0f / editor->camera.zoom);

    positionCircuitEntity(circuit, circuit->array + editor->tempCE_ID, pos);

    if (input->mouse.leftClick) {
      input->mouse.leftClick = 0;
      editor->state = EDIT_NONE;
    }
  }
  case EDIT_CREATE_WIRE: {
  }
  }

  if (editor->simulating) {
  }
}

void initEditorUI(Editor *editor) {
  UICtx *ctx = editor->uiCtx;
  SDL_Renderer *renderer = ctx->window->renderer;
  editor->menubarHeight = 80;
  setUICachedText(&editor->startSimulationText, renderer, ctx->font, "Simulate",
                  newColor(0, 0, 0, 255));
  setUICachedText(&editor->stopSimulationText, renderer, ctx->font, "Stop",
                  newColor(0, 0, 0, 255));

  setUICachedText(&editor->pivotText, renderer, ctx->font, "Pivot",
                  newColor(0, 0, 0, 255));

  setUICachedText(&editor->wireText, renderer, ctx->font, "Wire",
                  newColor(0, 0, 0, 255));

  setUICachedText(&editor->switchText, renderer, ctx->font, "Switch",
                  newColor(0, 0, 0, 255));
}
void initEditor(Editor *editor) {
  editor->state = EDIT_NONE;
  editor->simulating = 0;

  editor->camera.zoom = 1.0f;
  editor->camera.position = newVec2f(0.0f, 0.0f);
  editor->camera.oldPosition = newVec2f(0.0f, 0.0f);
  editor->camera.viewportSize.x = 1;
  editor->camera.viewportSize.y = 1;

  editor->bgColor = newColor(220, 220, 220, 0);
  editor->gridSize = 16;
  editor->selectBoxActive = 0;

  editor->editChipID = 0;
  editor->editChipNumInputs = 0;

  // keys control
  editor->zoomOutKey = SDL_SCANCODE_MINUS;
  editor->zoomInKey = SDL_SCANCODE_EQUALS;

  initEditorUI(editor);
}

void simulateButtonClicked(void *eventStateObject) {
  App *app = (App *)eventStateObject;
  Editor *editor = &app->editor;

  editor->simulating = !editor->simulating;
}
void closeEditChipMenu(void *eventStateObject) {
  App *app = (App *)eventStateObject;
  Editor *editor = &app->editor;

  editor->state = EDIT_NONE;
}

void createPivot(void *eventStateObject) {
  App *app = (App *)eventStateObject;
  Editor *editor = &app->editor;
  Ctx *ctx = editor->ctx;
  Circuit *circuit = &ctx->circuit;
  Pivots *pivots = &circuit->pivots;

  editor->tempCE_ID = pivotsNew(circuit);
  editor->state = EDIT_MOVE_CE;
}

void createWire(void *eventStateObject) {
  App *app = (App *)eventStateObject;
  Editor *editor = &app->editor;
  Ctx *ctx = editor->ctx;
  UICtx *uiCtx = editor->uiCtx;
  Circuit *circuit = &ctx->circuit;
  Wires *wires = &circuit->wires;

  editor->tempCE_ID = wiresNew(circuit);
  editor->state = EDIT_CREATE_WIRE;

  setUICachedText(&editor->editorMessage, uiCtx->window->renderer, uiCtx->font,
                  "select a pivot to bind to", newColor(0, 0, 0, 255));
}

void createSwitchChip(void *eventStateObject) {}

void editorUI(UICtx *uiCtx, Editor *editor) {
  // <navbar>
  uiBeginLayout(uiCtx,
                &(UILayoutOptions){.size.y = editor->menubarHeight,
                                   .padding = newVec4i(10, 10, 10, 10),
                                   .sizing = UI_FILL_WIDTH,
                                   .orientation = UI_HORIZONTAL,
                                   .bgColor = newColor(255, 255, 255, 255)});

  // <simulate button>
  Color simulateButtonColor = newColor(50, 200, 50, 255);
  UICachedText *simulateButtonText = &editor->startSimulationText;
  if (editor->simulating) {
    simulateButtonColor = newColor(200, 50, 50, 255);
    simulateButtonText = &editor->stopSimulationText;
  }

  uiBeginLayout(uiCtx, &(UILayoutOptions){.size = newVec2i(120, 50),
                                          .padding = newVec4i(10, 10, 10, 10),
                                          .bgColor = simulateButtonColor,

                                          .onClick = &simulateButtonClicked});

  uiLabel(uiCtx,
          &(UILabelOptions){.cachedText = simulateButtonText, .fontSize = 24});

  uiEndLayout(uiCtx);
  // </simulate button>

  // </navbar>

  uiEndLayout(uiCtx);

  // <editor message>
  if (editor->editorMessage.textLen > 0) {
    uiSetLayoutCursorPosX(uiCtx, uiRootLayout(uiCtx)->size.x / 2);
    uiLabel(uiCtx, &(UILabelOptions){.cachedText = &editor->editorMessage,
                                     .fontSize = 16});
    uiResetLayoutCursorX(uiCtx);
  }
  // </editor message>

  // <left sidebar>
  uiBeginLayout(uiCtx,
                &(UILayoutOptions){.size = newVec2i(90, 500),
                                   .padding = newVec4i(10, 10, 10, 10),
                                   .bgColor = newColor(200, 200, 200, 255),
                                   .spacing = 8});

  // <create pivot>
  uiBeginLayout(uiCtx,
                &(UILayoutOptions){.size = newVec2i(90, 22),
                                   .bgColor = newColor(255, 255, 255, 255),
                                   .padding = newVec4i(2, 2, 2, 2),
                                   .onClick = &createPivot});
  uiLabel(uiCtx,
          &(UILabelOptions){.cachedText = &editor->pivotText, .fontSize = 18});
  uiEndLayout(uiCtx);
  // </create pivot>

  // <create wire>
  uiBeginLayout(uiCtx,
                &(UILayoutOptions){.size = newVec2i(90, 22),
                                   .bgColor = newColor(255, 255, 255, 255),
                                   .padding = newVec4i(2, 2, 2, 2),
                                   .onClick = &createWire});
  uiLabel(uiCtx,
          &(UILabelOptions){.cachedText = &editor->wireText, .fontSize = 18});
  uiEndLayout(uiCtx);
  // </create wire>

  // <create switch>
  uiBeginLayout(uiCtx,
                &(UILayoutOptions){.size = newVec2i(90, 22),
                                   .bgColor = newColor(255, 255, 255, 255),
                                   .padding = newVec4i(2, 2, 2, 2),
                                   .onClick = &createSwitchChip});
  uiLabel(uiCtx,
          &(UILabelOptions){.cachedText = &editor->switchText, .fontSize = 18});
  uiEndLayout(uiCtx);
  // </create switch>

  uiEndLayout(uiCtx);
  // </left sidebar>

  if (editor->state == EDIT_SELECT_OPTION) {
    uiSetLayoutCursorPos(uiCtx, uiRootLayout(uiCtx)->size.x - 200,
                         15 + editor->menubarHeight);

    // edit options dashboard
    uiBeginLayout(uiCtx,
                  &(UILayoutOptions){.size = newVec2i(180, 500),
                                     .padding = newVec4i(10, 10, 10, 10),
                                     .bgColor = newColor(200, 200, 200, 255)});

    // <close button>
    uiMoveLayoutCursor(uiCtx, uiThisLayout(uiCtx)->size.x - 45, 0);
    uiBeginLayout(uiCtx,
                  &(UILayoutOptions){.size = newVec2i(32, 32),
                                     .padding = newVec4i(4, 4, 4, 4),
                                     .bgColor = newColor(200, 50, 50, 255),
                                     .onClick = &closeEditChipMenu});
    uiDecal(uiCtx, &(UIDecalOptions){.size = newVec2i(24, 24),
                                     .texture = uiCtx->defaultIcons.x});
    uiEndLayout(uiCtx);
    uiResetLayoutCursorX(uiCtx);
    // </close button>

    uiLabel(uiCtx, &(UILabelOptions){.cachedText = simulateButtonText,
                                     .fontSize = 24});

    uiLabel(uiCtx, &(UILabelOptions){.cachedText = simulateButtonText,
                                     .fontSize = 24});

    uiEndLayout(uiCtx);
  }

  /*u32 menubar = newUIElement(ui);
  ui->array[menubar].type = UI_BOX;
  ui->array[menubar].attachPosition = newVec2i(0,0);
  ui->array[menubar].size.y = editor->menubarHeight;
  ui->array[menubar].data.box.sizing.width = BOX_SIZING_WINDOW;
  ui->array[menubar].data.box.bgColor = newColor(225, 225, 225, 255);

  u32 simulateButton = newUIElement(ui);
  ui->array[simulateButton].parentID = menubar;
  ui->array[simulateButton].type = UI_BUTTON;
  ui->array[simulateButton].attachPosition = newVec2i(50, 50);
  ui->array[simulateButton].size = newVec2i(80, 40);
  ui->array[simulateButton].data.button.bgColor = newColor(50, 200, 50, 255);
  ui->array[simulateButton].data.button.onClick = &simulateButtonOnClick;*/
}

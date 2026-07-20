#include "editor.h"
#include "simulation/simulate.h"

#include "app.h"

#include <stdio.h>

void editorZoomOut(Editor *editor) {
  editor->camera.zoom += 0.1f;

  if (editor->camera.zoom >= 3.0f) {
    editor->camera.zoom = 3.0f;
    return;
  }
  editor->collisionStep = 1;
}
void editorZoomIn(Editor *editor) {
  editor->camera.zoom -= 0.1f;

  if (editor->camera.zoom <= 0.1f) {
    editor->camera.zoom = 0.1f;
    return;
  }
  editor->collisionStep = 1;
}

void editorHandleKeypress(Editor *editor, SDL_KeyboardEvent event) {
  SDL_Scancode code = event.scancode;
  if (code == editor->zoomInKey) {
    editorZoomIn(editor);
  } else if (code == editor->zoomOutKey) {
    editorZoomOut(editor);
  }
}

void checkCollisionsCE(Editor *editor, Input *input) {
  editor->hoveredCE_ID = 0;
  Circuit *circuit = &editor->ctx->circuit;

  for (u32 i = 1; i < circuit->pivots.len; i++) {
    Pivot *pivot = circuit->pivots.array + i;
    Vec2i pivotHitbox = scaleVec2i(newVec2i(15, 15), editor->camera.zoom);
    Vec2i screenPos = world2screenVec2i(editor->camera, pivot->position);

    if (cartesianCollideABB(input->mouse.position, screenPos, pivotHitbox)) {
      editor->hoveredCE_ID = pivot->ID;
      printf("HOVER %d\n", editor->hoveredCE_ID);
      return;
    }
  }
  for (u32 i = 1; i < circuit->inputChips.len; i++) {
    InputChip *inputChip = circuit->inputChips.array + i;
    Vec2i screenPos = world2screenVec2i(editor->camera, inputChip->position);
    Vec2i inputHitbox;

    switch (inputChip->type) {
    case SWITCH: {
      inputHitbox = newVec2i(50, 100);
      break;
    }
    default:
      break;
    }
    inputHitbox = scaleVec2i(inputHitbox, editor->camera.zoom);

    if (cartesianCollideABB(input->mouse.position, screenPos, inputHitbox)) {
      editor->hoveredCE_ID = inputChip->ID;
      printf("HOVER %d\n", editor->hoveredCE_ID);
      return;
    }
  }
}

void updateEditor(Editor *editor, Input *input) {
  Circuit *circuit = &editor->ctx->circuit;
  UICtx *uiCtx = editor->uiCtx;

  editor->collisionStep = input->mouse.positionUpdated;
  if (editor->collisionStep) {
    checkCollisionsCE(editor, input);
  }

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

  // temp ce as pointer
  CircuitEntity *ce = circuit->array + editor->tempCE_ID;
  CircuitEntity *hoveredCE = circuit->array + editor->hoveredCE_ID;

  if (editor->simulating) {
    if (editor->hoveredCE_ID == 0)
      return;
    if (!input->mouse.leftClick)
      return;

    ce = circuit->array + editor->hoveredCE_ID;
    if (ce->type == CE_INPUT) {
      InputChip *inputChip = circuit->inputChips.array + ce->typeID;
      inputChip->out = !inputChip->out;
    }
    return;
  }

  switch (editor->state) {
  case EDIT_NONE: {
    // keeps texture cached
    editor->editorMessage.textLen = 0;

    if (editor->hoveredCE_ID != 0 && input->mouse.leftClick) {
      editor->tempCE_ID = editor->hoveredCE_ID;
      ce = circuit->array + editor->hoveredCE_ID;

      editor->state = EDIT_SELECT_OPTION;
    }
    break;
  }
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
    break;
  }
  case EDIT_CREATE_WIRE: {
    setUICachedText(&editor->editorMessage, uiCtx->window->renderer,
                    uiCtx->font, "select a pivot to bind to",
                    newColor(0, 0, 0, 255));

    if (editor->hoveredCE_ID != 0 && input->mouse.leftClick == 1) {
      if (circuit->array[editor->hoveredCE_ID].type == CE_PIVOT) {
        Wire *wire = circuit->wires.array + ce->typeID;
        wire->pivotID1 = hoveredCE->typeID;
        editor->state = EDIT_SELECT_WIRE_PIVOT2;
        editor->editorMessageID = 0;
        printf("Connected pivot1! %d ", editor->hoveredCE_ID);
      }
    }
    break;
  }
  case EDIT_SELECT_WIRE_PIVOT2: {
    if (editor->editorMessageID == 0) {
      setUICachedText(&editor->editorMessage, uiCtx->window->renderer,
                      uiCtx->font, "select a 2nd pivot to bind to",
                      newColor(0, 0, 0, 255));
    } else if (editor->editorMessageID == 1) {
      setUICachedText(&editor->editorMessage, uiCtx->window->renderer,
                      uiCtx->font, "Cannot set pivot2 to same as pivot1",
                      newColor(255, 0, 0, 255));

      u32 current_time = SDL_GetTicks();
      if ((current_time - editor->editorMessageLastTime) / 1000.0f > 1) {
        editor->editorMessageID = 0;
      }
    }

    if (editor->hoveredCE_ID != 0 && input->mouse.leftClick == 1) {
      if (circuit->array[editor->hoveredCE_ID].type == CE_PIVOT) {
        Wire *wire = circuit->wires.array + ce->typeID;
        if (hoveredCE->typeID == wire->pivotID1) {
          editor->editorMessageID = 1;
          editor->editorMessageLastTime = SDL_GetTicks();
          break;
        }
        wire->pivotID2 = hoveredCE->typeID;

        editor->state = EDIT_NONE;

        printf("Connected pivot2! %d ", editor->hoveredCE_ID);
      }
    }
    break;
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
  setUICachedText(&editor->deleteText, renderer, ctx->font, "Delete",
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

  editor->tempCE_ID = 0;
  editor->hoveredCE_ID = 0;
  // DELETE {
  editor->editChipID = 0;
  editor->editChipNumInputs = 0;
  // }

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
  if (editor->state != EDIT_NONE)
    return;
  if (editor->simulating)
    return;

  Ctx *ctx = editor->ctx;
  Circuit *circuit = &ctx->circuit;
  Pivots *pivots = &circuit->pivots;

  editor->tempCE_ID = pivotsNew(circuit);
  editor->state = EDIT_MOVE_CE;
}

void createWire(void *eventStateObject) {
  App *app = (App *)eventStateObject;
  Editor *editor = &app->editor;
  if (editor->state != EDIT_NONE)
    return;
  if (editor->simulating)
    return;

  Ctx *ctx = editor->ctx;
  UICtx *uiCtx = editor->uiCtx;
  Circuit *circuit = &ctx->circuit;
  Wires *wires = &circuit->wires;

  editor->tempCE_ID = wiresNew(circuit);
  editor->state = EDIT_CREATE_WIRE;
}

void createSwitchChip(void *eventStateObject) {
  App *app = (App *)eventStateObject;
  Editor *editor = &app->editor;

  if (editor->state != EDIT_NONE)
    return;
  if (editor->simulating)
    return;

  Ctx *ctx = editor->ctx;
  UICtx *uiCtx = editor->uiCtx;
  Circuit *circuit = &ctx->circuit;
  Wires *wires = &circuit->wires;

  editor->tempCE_ID =
      inputChipsNew(circuit, &(InputChipOptions){.type = SWITCH});
  editor->state = EDIT_MOVE_CE;
}

void deleteButtonClicked(void *eventStateObject) {
  App *app = (App *)eventStateObject;
  Editor *editor = &app->editor;
  Ctx *ctx = editor->ctx;
  Circuit *circuit = &ctx->circuit;

  deleteCE(circuit, editor->tempCE_ID);
  editor->state = EDIT_NONE;
}

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
    uiSetLayoutCursorPos(uiCtx, 0, editor->menubarHeight);
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

    // <delete item button>
    uiBeginLayout(uiCtx,
                  &(UILayoutOptions){.size = newVec2i(96, 32),
                                     .padding = newVec4i(4, 4, 4, 4),
                                     .bgColor = newColor(200, 50, 50, 255),
                                     .onClick = &deleteButtonClicked});
    uiLabel(uiCtx, &(UILabelOptions){.cachedText = &editor->deleteText,
                                     .fontSize = 18});
    uiEndLayout(uiCtx);
    // </delete item button>

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

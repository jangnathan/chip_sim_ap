#include "editor.h"
#include "simulation/simulate.h"

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
    Vec2f mousePos = vec2ItoF(input->mouse.position);
    mousePos.x -= (float)editor->camera.viewportSize.x / 2.0f +
                  (float)editor->camera.viewportPos.x;
    mousePos.y = (float)(editor->camera.viewportSize.y + editor->camera.viewportPos.y) / 2.0f - mousePos.y;

    Vec2f pos = translateVec2f(editor->camera.position,
                                scaleVec2f(mousePos, 1.0f / editor->camera.zoom));

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
    simulate(editor->ctx);
  }
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
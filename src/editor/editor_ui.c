#include "editor/editor_ui.h"
#include "app.h"

#include <SDL3/SDL.h>

void initSimpleChipsText(UICtx *ctx, EditorUI *editorUI) {
  SDL_Renderer *renderer = ctx->window->renderer;

  for (u8 i = 0; i < SIMPLE_CHIP_TYPE_END; i++) {
    setUICachedText(editorUI->simpleChipsText + i, renderer, ctx->font,
		    SimpleChipsName[i], newColor(0, 0, 0, 255));
  }
}

void editorUI_init(UICtx *ctx, EditorUI *editorUI) {
  SDL_Renderer *renderer = ctx->window->renderer;
  editorUI->menubarHeight = 80;
  setUICachedText(&editorUI->startSimulationText, renderer, ctx->font,
		  "Simulate", newColor(0, 0, 0, 255));
  setUICachedText(&editorUI->stopSimulationText, renderer, ctx->font, "Stop",
		  newColor(0, 0, 0, 255));

  setUICachedText(&editorUI->pivotText, renderer, ctx->font, "Pivot",
		  newColor(0, 0, 0, 255));

  setUICachedText(&editorUI->wireText, renderer, ctx->font, "Wire",
		  newColor(0, 0, 0, 255));

  setUICachedText(&editorUI->switchText, renderer, ctx->font, "Switch",
		  newColor(0, 0, 0, 255));

  setUICachedText(&editorUI->deleteText, renderer, ctx->font, "Delete",
		  newColor(0, 0, 0, 255));

  initSimpleChipsText(ctx, editorUI);
}

Editor *getEditorFromESO(EventStateObject *eventStateObject) {
  EditorManager *manager = eventStateObject->manager;
  Editor *editor = manager->editors + manager->activeEditorIdx;

  return editor;
}

void simulateButtonClicked(void *eventStateObject, void *params) {
  EventStateObject *eventStateObj = (EventStateObject *)(eventStateObject);
  Editor *editor = getEditorFromESO(eventStateObj);

  editor->simulating = !editor->simulating;

  if (editor->simulating) {
    startSimulation(editor->ctx);
  } else {
    stopSimulation(editor->ctx);
  }
}
void closeEditChipMenu(void *eventStateObject, void *params) {
  EventStateObject *eventStateObj = (EventStateObject *)(eventStateObject);
  Editor *editor = getEditorFromESO(eventStateObj);

  editor->state = EDIT_NONE;
}

void createPivot(void *eventStateObject, void *params) {
  EventStateObject *eventStateObj = (EventStateObject *)(eventStateObject);
  Editor *editor = getEditorFromESO(eventStateObj);

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

void createWire(void *eventStateObject, void *params) {
  EventStateObject *eventStateObj = (EventStateObject *)(eventStateObject);
  Editor *editor = getEditorFromESO(eventStateObj);

  if (editor->state != EDIT_NONE)
    return;
  if (editor->simulating)
    return;

  Ctx *ctx = editor->ctx;
  Circuit *circuit = &ctx->circuit;
  Wires *wires = &circuit->wires;

  editor->tempCE_ID = wiresNew(circuit);
  editor->state = EDIT_CREATE_WIRE;
}

void createSwitchChip(void *eventStateObject, void *params) {
  EventStateObject *eventStateObj = (EventStateObject *)(eventStateObject);
  Editor *editor = getEditorFromESO(eventStateObj);

  if (editor->state != EDIT_NONE)
    return;
  if (editor->simulating)
    return;

  Ctx *ctx = editor->ctx;
  Circuit *circuit = &ctx->circuit;

  editor->tempCE_ID =
      inputChipsNew(circuit, &(InputChipOptions){.type = SWITCH});
  editor->state = EDIT_MOVE_CE;
}

typedef struct {
  SimpleChipType type;
} createSimpleChipParams;
void createSimpleChip(void *eventStateObject, void *params_a) {
  EventStateObject *eventStateObj = (EventStateObject *)(eventStateObject);
  createSimpleChipParams *params = (createSimpleChipParams *)params_a;

  Editor *editor = getEditorFromESO(eventStateObj);

  if (editor->state != EDIT_NONE)
    return;
  if (editor->simulating)
    return;

  Ctx *ctx = editor->ctx;
  Circuit *circuit = &ctx->circuit;

  editor->tempCE_ID =
      simpleChipsNew(circuit, &(SimpleChipOptions){.type = params->type});
  editor->state = EDIT_MOVE_CE;
}

void deleteButtonClicked(void *eventStateObject, void *param) {
  EventStateObject *eventStateObj = (EventStateObject *)(eventStateObject);
  Editor *editor = getEditorFromESO(eventStateObj);

  Ctx *ctx = editor->ctx;
  Circuit *circuit = &ctx->circuit;

  deleteCE(circuit, editor->tempCE_ID);
  editor->state = EDIT_NONE;
}

void renderSimpleChipBtnsUI(UICtx *uiCtx, Editor *editor, EditorUI *editorUI) {
  for (u8 i = 0; i < SIMPLE_CHIP_TYPE_END; i++) {
    createSimpleChipParams *params = &(createSimpleChipParams){.type = i};

    uiBeginLayout(uiCtx,
		  &(UILayoutOptions){.size = newVec2i(90, 22),
				     .bgColor = newColor(255, 255, 255, 255),
				     .padding = newVec4i(2, 2, 2, 2),
				     .onClick = &createSimpleChip,
				     .onClickParams = params,
				     .hoverCursorIcon = CURSOR_POINTER});
    uiLabel(uiCtx,
	    &(UILabelOptions){.cachedText = editorUI->simpleChipsText + i,
			      .fontSize = 18});
    uiEndLayout(uiCtx);
  }
}

void editorUI_run(UICtx *uiCtx, Editor *editor, EditorUI *editorUI) {
  // <navbar>
  uiBeginLayout(uiCtx,
		&(UILayoutOptions){.size.y = editorUI->menubarHeight,
				   .padding = newVec4i(10, 10, 10, 10),
				   .sizing = UI_FILL_WIDTH,
				   .orientation = UI_HORIZONTAL,
				   .bgColor = newColor(255, 255, 255, 255)});

  // <simulate button>
  Color simulateButtonColor = newColor(50, 200, 50, 255);
  UICachedText *simulateButtonText = &editorUI->startSimulationText;
  if (editor->simulating) {
    simulateButtonColor = newColor(200, 50, 50, 255);
    simulateButtonText = &editorUI->stopSimulationText;
  }

  uiBeginLayout(uiCtx, &(UILayoutOptions){.size = newVec2i(120, 50),
					  .padding = newVec4i(10, 10, 10, 10),
					  .bgColor = simulateButtonColor,

					  .onClick = &simulateButtonClicked,
					  .hoverCursorIcon = CURSOR_POINTER});

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
    uiSetLayoutCursorPos(uiCtx, 0, uiThisLayout(uiCtx)->cursorPos.y - 16);
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
				   .onClick = &createPivot,
				   .hoverCursorIcon = CURSOR_POINTER});
  uiLabel(uiCtx, &(UILabelOptions){.cachedText = &editorUI->pivotText,
				   .fontSize = 18});
  uiEndLayout(uiCtx);
  // </create pivot>

  // <create wire>
  uiBeginLayout(uiCtx,
		&(UILayoutOptions){.size = newVec2i(90, 22),
				   .bgColor = newColor(255, 255, 255, 255),
				   .padding = newVec4i(2, 2, 2, 2),
				   .onClick = &createWire,
				   .hoverCursorIcon = CURSOR_POINTER});
  uiLabel(uiCtx,
	  &(UILabelOptions){.cachedText = &editorUI->wireText, .fontSize = 18});
  uiEndLayout(uiCtx);
  // </create wire>

  // <create switch>
  uiBeginLayout(uiCtx,
		&(UILayoutOptions){.size = newVec2i(90, 22),
				   .bgColor = newColor(255, 255, 255, 255),
				   .padding = newVec4i(2, 2, 2, 2),
				   .onClick = &createSwitchChip,
				   .hoverCursorIcon = CURSOR_POINTER});
  uiLabel(uiCtx, &(UILabelOptions){.cachedText = &editorUI->switchText,
				   .fontSize = 18});
  uiEndLayout(uiCtx);
  // </create switch>

  renderSimpleChipBtnsUI(uiCtx, editor, editorUI);

  uiEndLayout(uiCtx);
  // </left sidebar>

  if (editor->state == EDIT_SELECT_OPTION) {
    uiSetLayoutCursorPos(uiCtx, uiRootLayout(uiCtx)->size.x - 200,
			 15 + editorUI->menubarHeight);

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
				     .onClick = &closeEditChipMenu,
				     .hoverCursorIcon = CURSOR_POINTER});
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
				     .onClick = &deleteButtonClicked,
				     .hoverCursorIcon = CURSOR_POINTER});
    uiLabel(uiCtx, &(UILabelOptions){.cachedText = &editorUI->deleteText,
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

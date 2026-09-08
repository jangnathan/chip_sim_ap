#include "editor/editor_ui.h"
#include "app.h"

#include <SDL3/SDL.h>

void editorUI_init(UICtx *ctx, EditorUI *editorUI) {
  SDL_Renderer *renderer = ctx->window->renderer;
  editorUI->menubarHeight = 80;
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

  strncpy(editor->editorMessage, "SELECT A PIVOT", MAX_TEXT_LEN);
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

void modeButtonClicked(void *eventStateObject, void *param) {
  EventStateObject *eventStateObj = (EventStateObject *)(eventStateObject);
  Editor *editor = getEditorFromESO(eventStateObj);

  if (editor->choosingEditorMode == 1) {
    editor->choosingEditorMode = 0;
  } else {
    editor->choosingEditorMode = 1;
  }
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
			      .fontSize = 18,
			      .text = SimpleChipsName[i]});
    uiEndLayout(uiCtx);
  }
}

typedef struct {
  EditMode mode;
} SelectEditorModeParams;

void selectEditorModeButton(void *eventStateObject, void *param) {
  EventStateObject *eventStateObj = (EventStateObject *)(eventStateObject);
  SelectEditorModeParams *params = (SelectEditorModeParams *)param;

  Editor *editor = getEditorFromESO(eventStateObj);

  editor->mode = params->mode;
  editor->choosingEditorMode = 0;
}

void editorUI_navbar(UICtx *uiCtx, Editor *editor, EditorUI *editorUI) {
  // <navbar>
  uiBeginLayout(uiCtx,
		&(UILayoutOptions){.size.y = editorUI->menubarHeight,
				   .padding = newVec4i(10, 10, 10, 10),
				   .sizing = UI_FILL_WIDTH,
				   .orientation = UI_HORIZONTAL,
				   .bgColor = newColor(255, 255, 255, 255)});

  uiBeginLayout(uiCtx,
		&(UILayoutOptions){.size = newVec2i(120, 50),
				   .bgColor = newColor(200, 200, 200, 255),
				   .orientation = UI_VERTICAL,
				   .spacing = 0});
  // <mode button>

  uiBeginLayout(uiCtx,
		&(UILayoutOptions){.size = newVec2i(120, 50),
				   .padding = newVec4i(10, 10, 10, 10),
				   .bgColor = newColor(200, 200, 200, 255),
				   .onClick = &modeButtonClicked,
				   .hoverCursorIcon = CURSOR_POINTER});

  if (editor->mode == EDIT_MODE_INTERIOR) {
    uiLabel(uiCtx, &(UILabelOptions){.cachedText = &editorUI->interiorText,
				     .fontSize = 24,
				     .text = "Interior"});
  } else if (editor->mode == EDIT_MODE_EXTERIOR) {
    uiLabel(uiCtx, &(UILabelOptions){.cachedText = &editorUI->exteriorText,
				     .fontSize = 24,
				     .text = "Exterior"});
  }
  uiEndLayout(uiCtx);

  // Editor mode selection dropdown
  if (editor->choosingEditorMode == 1) {
    // Interior button
    uiBeginLayout(uiCtx,
		  &(UILayoutOptions){.size = newVec2i(120, 50),
				     .bgColor = newColor(245, 245, 245, 255),
				     .padding = newVec4i(10, 10, 10, 10),
				     .hoverCursorIcon = CURSOR_POINTER,
				     .onClick = &selectEditorModeButton,
				     .onClickParams = &(SelectEditorModeParams){
					 .mode = EDIT_MODE_INTERIOR}});
    uiLabel(uiCtx, &(UILabelOptions){.cachedText = &editorUI->interiorText,
				     .fontSize = 24,
				     .text = "Interior"});
    uiEndLayout(uiCtx);

    // Exterior button
    uiBeginLayout(uiCtx,
		  &(UILayoutOptions){.size = newVec2i(120, 50),
				     .bgColor = newColor(245, 245, 245, 255),
				     .padding = newVec4i(10, 10, 10, 10),

				     .hoverCursorIcon = CURSOR_POINTER,
				     .onClick = &selectEditorModeButton,
				     .onClickParams = &(SelectEditorModeParams){
					 .mode = EDIT_MODE_EXTERIOR}});
    uiLabel(uiCtx, &(UILabelOptions){.cachedText = &editorUI->exteriorText,
				     .fontSize = 24,
				     .text = "Exterior"});
    uiEndLayout(uiCtx);
  }

  uiEndLayout(uiCtx);
  uiMoveLayoutCursor(uiCtx, 10, 0);
  // </mode button>

  // <simulate button>
  Color simulateButtonColor = newColor(50, 200, 50, 255);
  if (editor->simulating) {
    simulateButtonColor = newColor(200, 50, 50, 255);
  }

  uiBeginLayout(uiCtx, &(UILayoutOptions){.size = newVec2i(120, 50),
					  .padding = newVec4i(10, 10, 10, 10),
					  .bgColor = simulateButtonColor,

					  .onClick = &simulateButtonClicked,
					  .hoverCursorIcon = CURSOR_POINTER});

  if (editor->simulating) {
    uiLabel(uiCtx,
	    &(UILabelOptions){.cachedText = &editorUI->stopSimulationText,
			      .fontSize = 24,
			      .text = "Stop"});
  } else {
    uiLabel(uiCtx,
	    &(UILabelOptions){.cachedText = &editorUI->startSimulationText,
			      .fontSize = 24,
			      .text = "Simulate"});
  }

  uiEndLayout(uiCtx);
  // </simulate button>

  // Other buttons (will have logos)
  uiMoveLayoutCursor(uiCtx, 10, 0);
  uiBeginLayout(uiCtx,
		&(UILayoutOptions){.size = newVec2i(500, 50),
				   .padding = newVec4i(10, 10, 10, 10),
				   .bgColor = newColor(200, 200, 200, 255)});

  // manage chip pins button (opens manage chip button screen)

  uiBeginLayout(uiCtx,
		&(UILayoutOptions){.size = newVec2i(35, 35),
				   .padding = newVec4i(10, 10, 10, 10),
				   .bgColor = newColor(200, 200, 200, 255),
				   .hoverCursorIcon = CURSOR_POINTER});


  uiEndLayout(uiCtx);
  uiEndLayout(uiCtx);

  uiEndLayout(uiCtx);
  // </navbar>
}

void editorUI_mode_interior(UICtx *uiCtx, Editor *editor, EditorUI *editorUI) {
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
				   .fontSize = 18,
				   .text = "Pivot"});
  uiEndLayout(uiCtx);
  // </create pivot>

  // <create wire>
  uiBeginLayout(uiCtx,
		&(UILayoutOptions){.size = newVec2i(90, 22),
				   .bgColor = newColor(255, 255, 255, 255),
				   .padding = newVec4i(2, 2, 2, 2),
				   .onClick = &createWire,
				   .hoverCursorIcon = CURSOR_POINTER});
  uiLabel(uiCtx, &(UILabelOptions){.cachedText = &editorUI->wireText,
				   .fontSize = 18,
				   .text = "Wire"});
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
				   .fontSize = 18,
				   .text = "Switch"});
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
				     .fontSize = 18,
				     .text = "Delete"});
    uiEndLayout(uiCtx);
    // </delete item button>

    uiEndLayout(uiCtx);
  }
}

void editorUI_mode_exterior(UICtx *uiCtx, Editor *editor, EditorUI *editorUI) {}

void editorUI_run(UICtx *uiCtx, Editor *editor, EditorUI *editorUI) {
  // draw navbar
  uiSetLayoutCursorPos(uiCtx, 0, editorUI->menubarHeight);

  // <editor message>
  if (editor->editorMessage[0] != '\0') {
    uiSetLayoutCursorPosX(uiCtx, uiRootLayout(uiCtx)->size.x / 2);
    uiLabel(uiCtx, &(UILabelOptions){.cachedText = &editorUI->editorMessageText,
				     .fontSize = 16,
				     .text = editor->editorMessage});
    uiSetLayoutCursorPos(uiCtx, 0, editorUI->menubarHeight);
  }
  // </editor message>

  if (editor->mode == EDIT_MODE_INTERIOR) {
    editorUI_mode_interior(uiCtx, editor, editorUI);
  } else if (editor->mode == EDIT_MODE_EXTERIOR) {
    editorUI_mode_exterior(uiCtx, editor, editorUI);
  }

  // draw navbar
  uiSetLayoutCursorPos(uiCtx, 0, 0);
  editorUI_navbar(uiCtx, editor, editorUI);

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

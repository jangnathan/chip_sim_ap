#include "editor/render.h"
#include "editor/manager.h"
#include "app_state.h"

#include <stdlib.h>

void editorManagerInit(EditorManager *manager, UICtx *uiCtx) {
  manager->ctxArray = malloc(sizeof(Ctx) * 4);
  manager->ctxArrayLen = 0;
  manager->ctxArraySize = 4;

  manager->editors = malloc(sizeof(Editor) * 4);
  manager->editorsLen = 0;
  manager->editorsSize = 4;

  manager->activeEditorIdx = editorManagerAddEditor(manager);
}

void closeTab(void *eventStateObject, void *param) {
  u32 tabID = *((u32 *)param);

  printf("Close tab: %d\n", tabID);
}

void switchToTab(void *eventStateObject, void *param) {
  EventStateObject *eso = (EventStateObject *)eventStateObject;
  EditorManager *manager = eso->manager;

  u16 tabID = *((u32 *)param);

  manager->activeEditorIdx = tabID;
  printf("Opened tab: %d\n", tabID);
}

void editorManagerRender(SDL_Renderer *renderer, Textures *textures,
			 EditorManager *manager, EditorUI *editorUI,
			 UICtx *uiCtx) {

  // TODO: RENDER TABS FOR CIRCUITS also an add button to add circuit

  uiSetLayoutCursorPos(uiCtx, 0, 20 + 30);

  if (manager->activeEditorIdx != -1) {
    Editor *editor = manager->editors + manager->activeEditorIdx;
    renderEditor(renderer, textures, editor);
    editorUI_run(uiCtx, editor, editorUI);
  }

  uiSetLayoutCursorPos(uiCtx, 0, 20);
  uiBeginLayout(uiCtx,
		&(UILayoutOptions){.padding = newVec4i(2, 2, 2, 2),
				   .orientation = UI_HORIZONTAL,
				   .size.y = 30,
				   .sizing = UI_FILL_WIDTH,
				   .bgColor = newColor(210, 210, 210, 255)});

  // tabs
  for (u32 i = 0; i < manager->editorsLen; i++) {
    setUICachedText(manager->cachedText + i, uiCtx->window->renderer,
		    uiCtx->font, "new tab", newColor(0, 0, 0, 255));

    Color tabColor;
    if (i == manager->activeEditorIdx) {
      tabColor = newColor(200, 200, 225, 255);
    } else {
      tabColor = newColor(245, 245, 245, 255);
    }

    uiBeginLayout(uiCtx, &(UILayoutOptions){.bgColor = tabColor,
					    .size.y = 28,
					    .size.x = 100,
					    .padding = newVec4i(2, 2, 2, 2),
					    .spacing = 2,
					    .orientation = UI_HORIZONTAL,
					    .onClick = &switchToTab,
					    .onClickParams = &i});
    uiLabel(uiCtx, &(UILabelOptions){.cachedText = manager->cachedText + i,
				     .fontSize = 20});
    uiSetLayoutCursorPos(uiCtx,
			 uiThisLayout(uiCtx)->position.x +
			     uiThisLayout(uiCtx)->size.x - 20,
			 uiThisLayout(uiCtx)->position.y);

    // btn
    uiBeginLayout(uiCtx, &(UILayoutOptions){.size = newVec2i(20, 20),
					    .onClick = &closeTab,
					    .onClickParams = &i,
					    .padding = newVec4i(6, 0, 6, 0)});
    uiDecal(uiCtx, &(UIDecalOptions){.size = newVec2i(12, 12),
				     .texture = uiCtx->defaultIcons.x});
    uiEndLayout(uiCtx);

    uiEndLayout(uiCtx);
  }
  uiEndLayout(uiCtx);
}

void editorManagerUpdate(EditorManager *manager, Input *input, UICtx *uiCtx,
			 Vec2i windowSize) {
  if (manager->activeEditorIdx != -1) {
    Editor *editor = manager->editors + manager->activeEditorIdx;

    editor->camera.viewportSize = windowSize;
    editor->camera.viewportPos = newVec2i(0, 0);

    updateEditor(editor, input, uiCtx);
  }
}

void updateEditorsCtx(EditorManager *manager) {
  for (u16 i = 0; i < manager->editorsLen; i++) {
    manager->editors[i].ctx = manager->ctxArray + manager->editors[i].ctxID;
  }
}

u16 editorManagerAddEditor(EditorManager *manager) {
  u16 newEditorID = manager->editorsLen;
  manager->editorsLen++;
  if (manager->editorsLen >= manager->editorsSize) {
    manager->editorsSize = manager->editorsSize * 2;
    manager->editors =
	realloc(manager->editors, sizeof(Editor) * manager->editorsSize);
  }

  u16 newCtxID = manager->ctxArrayLen;
  manager->ctxArrayLen++;
  if (manager->ctxArrayLen >= manager->ctxArraySize) {
    manager->ctxArraySize = manager->ctxArraySize * 2;
    manager->ctxArray =
	realloc(manager->ctxArray, sizeof(Ctx) * manager->ctxArraySize);

    updateEditorsCtx(manager);
  }

  ctxInit(manager->ctxArray + newCtxID);

  manager->editors[newEditorID].ctxID = newCtxID;
  manager->editors[newEditorID].ctx = manager->ctxArray + newCtxID;
  initEditor(manager->editors + newEditorID);

  return newEditorID;
}

void editorManagerFree(EditorManager *manager) {
  for (u16 i = 0; i < manager->ctxArrayLen; i++) {
    ctxFree(manager->ctxArray + i);
  }
  free(manager->ctxArray);

  free(manager->editors);
}

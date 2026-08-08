#include "editor/manager.h"

#include <stdlib.h>

void editorManagerInit(EditorManager *manager, UICtx *uiCtx) {
  manager->ctxArray = malloc(sizeof(Ctx) * 4);
  manager->ctxArrayLen = 1;
  manager->ctxArraySize = 4;

  ctxInit(manager->ctxArray);

  manager->editors = malloc(sizeof(Editor) * 4);
  manager->editorsLen = 1;
  manager->editorsSize = 4;

  Editor *firstEditor = manager->editors;
  firstEditor->ctx = manager->ctxArray;
  firstEditor->uiCtx = uiCtx;

  initEditor(firstEditor); // init the first

  manager->activeEditorIdx = 0;
}

void editorManagerRender(SDL_Renderer *renderer, Textures *textures,
                         EditorManager *manager, EditorUI *editorUI,
                         UICtx *uiCtx) {

  if (manager->activeEditorIdx != -1) {
    Editor *editor = manager->editors + manager->activeEditorIdx;
    renderEditor(renderer, textures,
                 editor);
    editorUI_run(uiCtx, editor, editorUI);
  }
}

void editorManagerUpdate(EditorManager *manager, Input *input, Vec2i windowSize) {
  if (manager->activeEditorIdx != -1) {
    Editor *editor = manager->editors + manager->activeEditorIdx;

    editor->camera.viewportSize = windowSize;
    editor->camera.viewportPos = newVec2i(0, 0);

    updateEditor(editor, input);
  }
}

void editorManagerFree(EditorManager *manager) {
  for (u16 i = 0; i < manager->ctxArrayLen; i++) {
    ctxFree(manager->ctxArray + i);
  }
  free(manager->ctxArray);

  free(manager->editors);
}
#pragma once

#include "simulation/ctx.h"
#include "editor/editor.h"
#include "editor/editor_ui.h"
#include "core/input.h"
#include "textures.h"
#include "constants.h"

#include <SDL3/SDL.h>

typedef struct {
    Ctx *ctxArray;
    u16 ctxArrayLen;
    u16 ctxArraySize;

    Editor *editors;
    u16 editorsLen;
    u16 editorsSize;

    UICachedText cachedText[16];

    i32 activeEditorIdx;
} EditorManager;

void editorManagerInit(EditorManager *manager, UICtx *uiCtx);
void editorManagerRender(SDL_Renderer *renderer, Textures *textures, EditorManager *manager, EditorUI *editorUI, UICtx *uiCtx);
void editorManagerUpdate(EditorManager *manager, Input *input, UICtx *uiCtx, Vec2i windowSize);

u16 editorManagerAddCtx(EditorManager *manager);
u16 editorManagerAddEditor(EditorManager *manager, u16 ctxID);

u8 editorManagerDeleteCtx(EditorManager *manager, u16 ID);
u8 editorManagerDeleteEditor(EditorManager *manager, u16 ID);

void editorManagerFree(EditorManager *manager);
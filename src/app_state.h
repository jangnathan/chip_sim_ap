#pragma once

#include "editor/editor.h"
#include "editor/manager.h"
#include "core/window.h"
#include "simulation/ctx.h"

typedef enum {
	ST_NONE,
	ST_EDIT
} AppState;

typedef struct {
    EditorManager *manager;
	tWindow *window;
} EventStateObject;
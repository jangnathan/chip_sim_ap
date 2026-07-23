#pragma once

#include "editor/editor.h"
#include "simulation/ctx.h"

typedef enum {
	ST_NONE,
	ST_EDIT
} AppState;

typedef struct {
    Editor *editor;
    Ctx *ctx;

    // params
    u32 simpleChipUI_i;
} EventStateObject;
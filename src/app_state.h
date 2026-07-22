#include "editor/editor.h"

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
#pragma once
#include "editor.h"
#include "textures.h"

#include <SDL3/SDL.h>

void renderEditor(SDL_Renderer *renderer, Textures *textures, i32 winWidth, Editor *editor, Chips *chips);

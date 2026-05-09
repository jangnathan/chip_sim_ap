#pragma once
#include <SDL3/SDL.h>
#include "app.h"

void render(App *app);

UITextInput setUITextInput(App *app, UITextInput textInput, char *str);

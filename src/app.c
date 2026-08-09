#include "app.h"
#include "core/event.h"
#include "core/input.h"
#include "editor/render.h"
#include "simulation/circuit.h"
#include "simulation/simulate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720

void initApp(App *app) {
  u8 sucess = SDL_Init(SDL_INIT_VIDEO);
  if (!sucess) {
    fprintf(stderr, "error initializing SDL3");
    exit(1);
  }

  if (!TTF_Init()) {
    fprintf(stderr, "error initializing SDL3_ttf");
    exit(1);
  }

  app->font = TTF_OpenFont("asset/ARIAL.ttf", 16);
  if (!app->font) {
    fprintf(stderr, "error loading font");
    exit(1);
  }

  app->running = 1;
  createWindow(&app->window, newVec2i(WINDOW_WIDTH, WINDOW_HEIGHT));
  windowSetMinSize(&app->window, newVec2i(500, 300));

  Input *input = &app->input;
  initInput(input);

  app->uiCtx.window = &app->window;
  app->uiCtx.input = input;
  app->uiCtx.font = app->font;

  app->eventStateObject.manager = &app->editorManager;
  app->uiCtx.eventStateObject = &app->eventStateObject;
  initUICtx(&app->uiCtx);

  initMenubar(&app->menubar, &app->uiCtx);

  app->state = ST_EDIT;
  editorManagerInit(&app->editorManager, &app->uiCtx);

  editorUI_init(&app->uiCtx, &app->editorUI);

  loadTextures(app->window.renderer, &app->textures, app->font);

  printf("All initialized\n");
  // app->ctx.circuit.len = 0;
}

void update(App *app) {
  updateWindow(&app->window);

  switch (app->state) {
  case ST_NONE:
    break;
  case ST_EDIT: {
    Vec2i viewportSize = app->window.size;
    editorManagerUpdate(&app->editorManager, &app->input, &app->uiCtx, viewportSize);
    break;
  }
  }
}

void render(App *app) {
  SDL_Renderer *renderer = app->window.renderer;
  UICtx *uiCtx = &app->uiCtx;

  SDL_SetRenderDrawColor(renderer, 225, 225, 225, 255);
  SDL_RenderClear(renderer);

  uiBeginRoot(uiCtx);

  // app ui
  uiSetLayoutCursorPos(uiCtx, 0, 20);

  switch (app->state) {
  case ST_NONE:
    break;
  case ST_EDIT:
    editorManagerRender(renderer, &app->textures, &app->editorManager, &app->editorUI, uiCtx);
    break;
  }

  // menubar is at top
  uiSetLayoutCursorPos(uiCtx, 0, 0);
  menubarUI(&app->menubar, uiCtx);

  uiEndRoot(uiCtx);

  SDL_RenderPresent(renderer);
}

void closeApp(App *app) {
  closeInput(&app->input);
  closeWindow(&app->window);
  editorManagerFree(&app->editorManager);

  SDL_Quit();
}

void runApp(App *app) {
  app->running = 1;

  while (app->running) {
    handleEvents(app);
    update(app);
    render(app);
    SDL_Delay(33);
  }

  closeApp(app);
}

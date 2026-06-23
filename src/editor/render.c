#include "render.h"
#include <math.h>

typedef struct {
  SDL_Renderer *renderer;
  Textures *textures;
  Camera camera;
} RendererCtx;

SDL_FRect createRenderRect(Camera camera, float x, float y, float width,
                           float height) {
  Vec2i screenPos = world2screenVec2i(camera, newVec2f(x, y));
  SDL_FRect rect = {screenPos.x - width * camera.zoom / 2,
                    screenPos.y - height * camera.zoom / 2, width * camera.zoom,
                    height * camera.zoom};
  return rect;
}

void drawSwitch(RendererCtx *renderCtx, u8 on, Vec2f pos);
// PIVOT
void drawPivot(RendererCtx *renderCtx, Pivot *pivot) {
  SDL_Renderer *renderer = renderCtx->renderer;
  Camera camera = renderCtx->camera;

  Vec2f pos = pivot->position;

  SDL_FRect rect = createRenderRect(camera, pos.x, pos.y, 15.0f, 15.0f);
  SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255);
  SDL_RenderFillRect(renderer, &rect);
}

// INPUT CHIP

void drawSwitch(RendererCtx *renderCtx, u8 on, Vec2f pos) {
  SDL_Renderer *renderer = renderCtx->renderer;
  Camera camera = renderCtx->camera;

  float x = pos.x;
  float y = pos.y;
  SDL_FRect indicator = createRenderRect(camera, x, y + 50.0f, 50.0f, 10.0f);
  SDL_FRect box = createRenderRect(camera, x, y, 50.0f, 90.0f);
  SDL_FRect switchPath = createRenderRect(camera, x, y, 40.0f, 80.0f);
  SDL_FRect switchBox =
      createRenderRect(camera, x, y - 20.0f + (on * 40.0f), 40.0f, 40.0f);

  if (on) {
    SDL_SetRenderDrawColor(renderer, 205, 20, 20, 0);
  } else {
    SDL_SetRenderDrawColor(renderer, 80, 30, 30, 0);
  }
  SDL_RenderFillRect(renderer, &indicator);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
  SDL_RenderFillRect(renderer, &box);

  SDL_SetRenderDrawColor(renderer, 205, 205, 205, 0);
  SDL_RenderFillRect(renderer, &switchPath);

  SDL_SetRenderDrawColor(renderer, 50, 50, 50, 0);
  SDL_RenderFillRect(renderer, &switchBox);
}

void drawClock(RendererCtx *renderCtx, Vec2f pos) {
  SDL_Renderer *renderer = renderCtx->renderer;
  Camera camera = renderCtx->camera;

  float x = pos.x;
  float y = pos.y;

  SDL_FRect background = createRenderRect(camera, x, y, 50.0f, 50.0f);
  SDL_FRect inside = createRenderRect(camera, x + 2.0f, y + 2.0f, 46.0f, 46.0f);
  SDL_SetRenderDrawColor(renderer, 50, 50, 50, 0);
  SDL_RenderFillRect(renderer, &background);

  // use a clock texture
}

void renderInputChip(RendererCtx *renderCtx, InputChip *inputChip) {
  switch (inputChip->type) {
  case CLOCK:
    drawClock(renderCtx, inputChip->position);
    break;
  case SWITCH:
    drawSwitch(renderCtx, inputChip->out, inputChip->position);
    break;
  }
}

// SIMPLE CHIP

void renderSimpleChip(RendererCtx *renderCtx, SimpleChip *simpleChip) {
  SDL_Renderer *renderer = renderCtx->renderer;
  Textures *textures = renderCtx->textures;
  Camera camera = renderCtx->camera;

  Vec2f pos = simpleChip->position;

  SDL_FRect box = createRenderRect(camera, pos.x, pos.y, 50.0f, 50.0f);
  SDL_SetRenderDrawColor(renderer, 25, 25, 25, 0);
  SDL_RenderFillRect(renderer, &box);

  SDL_FRect text = createRenderRect(camera, pos.x, pos.y, 40.0f, 20.0f);
  switch (simpleChip->type) {
  case AND:
    SDL_RenderTexture(renderer, textures->simpleAND, NULL, &text);
    break;
  case OR:
    SDL_RenderTexture(renderer, textures->simpleOR, NULL, &text);
    break;
  case NOT:
    SDL_RenderTexture(renderer, textures->simpleNOT, NULL, &text);
    break;
  case NAND:
    SDL_RenderTexture(renderer, textures->simpleNAND, NULL, &text);
    break;
  case NOR:
    SDL_RenderTexture(renderer, textures->simpleNOR, NULL, &text);
    break;
  case XOR:
    SDL_RenderTexture(renderer, textures->simpleXOR, NULL, &text);
    break;
  case XNOR:
    SDL_RenderTexture(renderer, textures->simpleXNOR, NULL, &text);
    break;
  }

  SDL_FRect indicator =
      createRenderRect(camera, pos.x, pos.y + 30, 50.0f, 10.0f);

  if (simpleChip->out) {
    SDL_SetRenderDrawColor(renderer, 205, 20, 20, 0);
  } else {
    SDL_SetRenderDrawColor(renderer, 80, 30, 30, 0);
  }
  SDL_RenderFillRect(renderer, &indicator);
}

void renderChip(RendererCtx *renderCtx, Circuit *circuit, u32 i) {
  u32 typeID = circuit->array[i].typeID;

  switch (circuit->array[i].type) {
  case CE_NONE:
    break;
  case CE_SIMPLE:
    renderSimpleChip(renderCtx, circuit->simpleChips.array + typeID);
    break;
  case CE_INPUT:
    renderInputChip(renderCtx, circuit->inputChips.array + typeID);
    break;
  }
}

// GRID

void renderGrid(SDL_Renderer *renderer, Editor *editor, float x, float y,
                float w, float h) {
  SDL_SetRenderDrawColor(renderer, 180, 180, 180, editor->bgColor.a);

  for (u16 i = 1; i < h / editor->gridSize; i++) {
    // line across x axis
    SDL_FRect lineX = {0,
                       i * editor->gridSize -
                           ((i32)editor->camera.position.y % editor->gridSize) *
                               editor->camera.zoom +
                           y,
                       w, 1.0f};
    SDL_RenderFillRect(renderer, &lineX);
  }
  for (u16 i = 1; i < w / editor->gridSize; i++) {
    SDL_FRect lineY = {i * editor->gridSize -
                           ((i32)editor->camera.position.x % editor->gridSize) *
                               editor->camera.zoom +
                           x,
                       y, 1.0f, h};
    SDL_RenderFillRect(renderer, &lineY);
  }
}

void renderEditor(SDL_Renderer *renderer, Textures *textures, Editor *editor) {
  Camera camera = editor->camera;
  Circuit *circuit = &editor->ctx->circuit;

  RendererCtx renderCtx;
  renderCtx.camera = editor->camera;
  renderCtx.renderer = renderer;
  renderCtx.textures = textures;

  Color bgColor = editor->bgColor;

  renderGrid(renderer, editor, 0, (float)editor->menubarHeight,
             (float)camera.viewportSize.x, (float)camera.viewportSize.y);

  // render circuit
  for (u32 i = 1; i < circuit->pivots.len; i++) {
    drawPivot(&renderCtx, circuit->pivots.array + i);
  }

  if (editor->selectBoxActive) {
    SDL_FRect box = {editor->selectBoxPos.x - editor->selectBoxSize.x / 2,
                     editor->selectBoxPos.y - editor->selectBoxSize.y / 2,
                     editor->selectBoxSize.x, editor->selectBoxSize.y};
    SDL_SetRenderDrawColor(renderer, 50, 50, 255, 0);
    SDL_RenderRect(renderer, &box);
  }
}

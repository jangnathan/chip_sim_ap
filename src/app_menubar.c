#include "app_menubar.h"
#include "editor/manager.h"
#include "app_state.h"
#include "constants.h"

void initMenubar(Menubar *menubar, UICtx *ctx) {
  setUICachedText(&menubar->fileText, ctx->window->renderer, ctx->font, "FILE",
                  newColor(0, 0, 0, 255));
  setUICachedText(&menubar->newText, ctx->window->renderer, ctx->font, "NEW",
                  newColor(0, 0, 0, 255));

  menubar->fileSectionActiveDelay = 0;
}

void menubarNewClicked(void *eventStateObject, void *param) {
  EventStateObject *evo = (EventStateObject*)eventStateObject;
  EditorManager *manager = evo->manager;

  editorManagerAddEditor(manager);
}

void menubarUI(Menubar *menubar, UICtx *uiCtx) {
  uiBeginLayout(uiCtx, &(UILayoutOptions){
                           .sizing = UI_FILL_WIDTH,
                           .orientation = UI_HORIZONTAL,
                           //.bgColor = newColor(100, 100, 100, 1)
                       });

  // <column>
  uiBeginLayout(uiCtx, &(UILayoutOptions){.orientation = UI_VERTICAL,
                                          .size.x = 16 * 3 + 8,
                                          .size.y = 20 * 2});

  uiBeginLayout(uiCtx, &(UILayoutOptions){.padding.t = 2,
                                          .padding.l = 5,

                                          .size.x = 16 * 3 + 8,
                                          .size.y = 20,
                                          .bgColor = newColor(200, 200, 200, 1),
                                          .isHoveredPtr = &menubar->fileButtonHovered});
  uiLabel(uiCtx,
          &(UILabelOptions){.cachedText = &menubar->fileText, .fontSize = 16});
  uiEndLayout(uiCtx);

  if (menubar->fileSectionActiveDelay > 0) {
    menubar->fileSectionActiveDelay--;

    uiBeginLayout(uiCtx,
                  &(UILayoutOptions){.padding.t = 2,
                                     .padding.l = 5,

                                     .size.x = 16 * 3 + 8,
                                     .size.y = 20,
                                     .bgColor = newColor(200, 200, 200, 1),
                                     .onClick = &menubarNewClicked,
                                     .isHoveredPtr = &menubar->fileButtonHovered});
    uiLabel(uiCtx,
            &(UILabelOptions){.cachedText = &menubar->newText, .fontSize = 16});
    uiEndLayout(uiCtx);
  }

  if (menubar->fileButtonHovered) {
    menubar->fileSectionActiveDelay = 2;
  }
  menubar->fileButtonHovered = 0;


  uiEndLayout(uiCtx);
  // <column/>

  uiEndLayout(uiCtx);
}
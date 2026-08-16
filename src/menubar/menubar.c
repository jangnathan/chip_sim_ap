#include "menubar/menubar.h"
#include "app_state.h"
#include "constants.h"
#include "editor/manager.h"
#include "menubar/menubar_file.h"

void initMenubar(Menubar *menubar, UICtx *ctx) {
  setUICachedText(&menubar->fileText, ctx->window->renderer, ctx->font, "FILE",
		  newColor(0, 0, 0, 255));
  setUICachedText(&menubar->newText, ctx->window->renderer, ctx->font, "NEW",
		  newColor(0, 0, 0, 255));
  setUICachedText(&menubar->saveFileText, ctx->window->renderer, ctx->font,
		  "SAVE FILE", newColor(0, 0, 0, 255));
  setUICachedText(&menubar->openFileText, ctx->window->renderer, ctx->font,
		  "OPEN FILE", newColor(0, 0, 0, 255));

  menubar->fileSectionActiveDelay = 0;
  menubar->hoverID = 0;
}

void menubarNewClicked(void *eventStateObject, void *param) {
  EventStateObject *evo = (EventStateObject *)eventStateObject;
  EditorManager *manager = evo->manager;

  u16 ctxID = editorManagerAddCtx(manager);
  editorManagerAddEditor(manager, ctxID);
}

void menubarSaveFileClicked(void *eventStateObject, void *param) {
  EventStateObject *evo = (EventStateObject *)eventStateObject;
  tWindow *window = evo->window;
  EditorManager *manager = evo->manager;

  chipSim_saveAsFile(manager, window);
}

void menubarOpenFileClicked(void *eventStateObject, void *param) {
  EventStateObject *evo = (EventStateObject *)eventStateObject;
  EditorManager *manager = evo->manager;

  u16 ctxID = editorManagerAddCtx(manager);
  editorManagerAddEditor(manager, ctxID);
}

typedef struct {
  u16 *hoverID_ptr;
  u16 tempID;
} MenubarHoverParam;

void menubarHover(void *EventStateObject, void *param) {
  MenubarHoverParam *menubarParam = (MenubarHoverParam *)param;

  *(menubarParam->hoverID_ptr) = menubarParam->tempID;
}

Color menubarItemColor(Menubar *menubar, u16 hoverID) {
  if (menubar->hoverID == hoverID) {
    return newColor(180, 180, 180, 255);
  } else {
    return newColor(200, 200, 200, 255);
  }
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

  u16 hoverID = 1;
  uiBeginLayout(uiCtx,
		&(UILayoutOptions){
		    .padding.t = 2,
		    .padding.l = 5,

		    .size.x = 16 * 3 + 8,
		    .size.y = 20,
		    .bgColor = menubarItemColor(menubar, hoverID),
		    .isHoveredPtr = &menubar->fileButtonHovered,
		    .onHover = &menubarHover,
		    .hoverParams = &(MenubarHoverParam){
			.hoverID_ptr = &menubar->hoverID, .tempID = hoverID}});
  uiLabel(uiCtx,
	  &(UILabelOptions){.cachedText = &menubar->fileText, .fontSize = 16});
  uiEndLayout(uiCtx);
  hoverID++;

  if (menubar->fileSectionActiveDelay > 0) {
    menubar->fileSectionActiveDelay--;

    uiBeginLayout(
	uiCtx, &(UILayoutOptions){
		   .padding.t = 2,
		   .padding.l = 5,

		   .size.x = 16 * 3 + 8,
		   .size.y = 20,
		   .onClick = &menubarNewClicked,
		   .bgColor = menubarItemColor(menubar, hoverID),
		   .isHoveredPtr = &menubar->fileButtonHovered,
		   .onHover = &menubarHover,
		   .hoverParams = &(MenubarHoverParam){
		       .hoverID_ptr = &menubar->hoverID, .tempID = hoverID}});
    uiLabel(uiCtx,
	    &(UILabelOptions){.cachedText = &menubar->newText, .fontSize = 16});
    uiEndLayout(uiCtx);
    hoverID++;

    uiBeginLayout(
	uiCtx, &(UILayoutOptions){
		   .padding.t = 2,
		   .padding.l = 5,

		   .size.x = 16 * 6 + 8,
		   .size.y = 20,
		   .onClick = &menubarSaveFileClicked,
		   .bgColor = menubarItemColor(menubar, hoverID),
		   .isHoveredPtr = &menubar->fileButtonHovered,
		   .onHover = &menubarHover,
		   .hoverParams = &(MenubarHoverParam){
		       .hoverID_ptr = &menubar->hoverID, .tempID = hoverID}});

    uiLabel(uiCtx, &(UILabelOptions){.cachedText = &menubar->saveFileText,
				     .fontSize = 16});
    uiEndLayout(uiCtx);
    hoverID++;

    uiBeginLayout(
	uiCtx, &(UILayoutOptions){
		   .padding.t = 2,
		   .padding.l = 5,

		   .size.x = 16 * 6 + 8,
		   .size.y = 20,
		   .bgColor = newColor(200, 200, 200, 1),
		   .onClick = &menubarOpenFileClicked,
		   .bgColor = menubarItemColor(menubar, hoverID),
		   .isHoveredPtr = &menubar->fileButtonHovered,
		   .onHover = &menubarHover,
		   .hoverParams = &(MenubarHoverParam){
		       .hoverID_ptr = &menubar->hoverID, .tempID = hoverID}});
    uiLabel(uiCtx, &(UILabelOptions){.cachedText = &menubar->openFileText,
				     .fontSize = 16});
    uiEndLayout(uiCtx);
    hoverID++;
  }

  if (menubar->fileButtonHovered) {
    menubar->fileSectionActiveDelay = 1;
  }
  menubar->fileButtonHovered = 0;

  uiEndLayout(uiCtx);
  // <column/>

  uiEndLayout(uiCtx);
}
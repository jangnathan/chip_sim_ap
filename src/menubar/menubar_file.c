#include "menubar/menubar_file.h"
#include "serial/serialize.h"
#include <SDL3/SDL.h>
#include <stdio.h>

void SDLCALL saveAsFileCallback(void *userdata, const char *const *filelist,
				int filter) {
  if (filelist == NULL) {
    SDL_Log("An error occurred: %s", SDL_GetError());
    return;
  }

  if (filelist[0] == NULL) {
    SDL_Log("User canceled the dialog.");
    return;
  }

  SDL_Log("Selected file: %s", filelist[0]);

  EditorManager *manager = (EditorManager*)userdata;

  FILE *fptr = fopen(filelist[0], "wb");
  Ctx *currentCtx = manager->editors[manager->activeEditorIdx].ctx;
  serializeCtx(currentCtx, fptr);
  fclose(fptr);
}

void chipSim_saveAsFile(EditorManager *manager, tWindow *window) {
  const SDL_DialogFileFilter filters[] = {{"chip sim files", "chipsim"}};

  SDL_ShowSaveFileDialog(saveAsFileCallback, manager, window->sdlWindow, filters,
			 1,   // num filters
			 NULL // starting path
  );
}
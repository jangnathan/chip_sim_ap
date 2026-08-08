#include "app.h"
#include "simulation/circuit.h"

#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  printf("Hello World!");
  // init
  App *app = malloc(sizeof(App));
  initApp(app);

  runApp(app);

  /*char h;
  printf("press any key to exit...");
  scanf("%c", &h);*/
  free(app);

  return 0;
}

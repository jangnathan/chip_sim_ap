#include "app.h"
#include "simulation/circuit.h"

#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("Hello World!");
  // init
  App *app = malloc(sizeof(App));
  initApp(app);

  Ctx *ctx = &app->ctx;
  Circuit *circuit = &ctx->circuit;

  u32 a = inputChipsNew(circuit, &(InputChipOptions){
	.type = SWITCH,
	.position = newVec2f(0, 0)
                                });

  runApp(app);
  free(app);

  return 0;
}

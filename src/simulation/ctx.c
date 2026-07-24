#include "simulation/ctx.h"
#include <stdlib.h>

void ctxInit(Ctx *ctx) {
    circuitInit(&ctx->circuit);
    connectionsInit(&ctx->connections);
}

void ctxFree(Ctx *ctx) {
    circuitFree(&ctx->circuit);
    connectionsFree(&ctx->connections);
}
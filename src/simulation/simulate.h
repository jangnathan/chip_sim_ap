#pragma once
#include "constants.h"
#include "ctx.h"

typedef struct {
    u8 *array;
    u32 len;
    u32 size;
} Connections;

void connectionsInit(Connections *connections);
u32 connectionsNew();

void startSimulation(Ctx *ctx);
void simulate(Ctx *ctx);
void stopSimulation(Ctx *ctx);
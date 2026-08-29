#pragma once
#include "constants.h"
#include "ctx.h"

// 2026
void generateConnections(Ctx *ctx);

void startSimulation(Ctx *ctx);
void simulate(Ctx *ctx);
void stopSimulation(Ctx *ctx);
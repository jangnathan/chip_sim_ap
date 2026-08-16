#pragma once
#include "simulation/circuit.h"
#include "connection.h"

typedef struct {
	Circuit circuit;
	Connections connections;

	char name[32];
} Ctx;

void ctxInit(Ctx *ctx);
void ctxFree(Ctx *ctx);
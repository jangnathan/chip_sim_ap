#pragma once
#include "connection.h"
#include "simulation/circuit.h"

typedef struct {
  char path[64];
  char identifier[32];
  u32 ctxID_psd;

  // SoA array to track
  u32 *designatedPivotIDs;
  u32 designatedPivotIDs_len;
  u32 designatedPivotIDs_size;
} CircuitDependency;

typedef struct {
  CircuitDependency *array;
  u16 len;
  u16 size;
} CircuitDependencies;

typedef struct {
  Circuit circuit;
  Connections connections;

  char identifier[32];

  CircuitDependencies dependencies;
} Ctx;

void ctxInit(Ctx *ctx);
void ctxFree(Ctx *ctx);
#include "simulation/simulate.h"
#include <stdlib.h>

void resetConnections(Ctx *ctx) {
  Circuit *circuit = &ctx->circuit;
  Connections *connections = &ctx->connections;
  connections->len = 1;

  for (u32 pivot_i = 1; pivot_i < circuit->pivots.len; pivot_i++) {
    Pivot *pivot = circuit->pivots.array + pivot_i;

    pivot->connectionID = 0;
  }
}

u8 checkAndGenerateConnection(Ctx *ctx, u32 targetPivotID, u32 wirePivotID1,
                              u32 wirePivotID2) {
  Circuit *circuit = &ctx->circuit;
  Pivots *pivots = &circuit->pivots;

  // return check
  if (wirePivotID1 == targetPivotID) {
    // if pivotID2 has connection, set pivotID1 to it
    // if it does not, create new connection, then set both pivots to it

    if (pivots->array[wirePivotID2].connectionID == 0) {
      u32 newConnectionID = connectionsNew(&ctx->connections);

      pivots->array[wirePivotID1].connectionID = newConnectionID;
      pivots->array[wirePivotID2].connectionID = newConnectionID;
    } else {
      pivots->array[wirePivotID1].connectionID =
          pivots->array[wirePivotID2].connectionID;
    }

    return 1;
  }

  return 0;
}

void generateConnections(Ctx *ctx) {
  Circuit *circuit = &ctx->circuit;
  resetConnections(ctx);

  for (u32 pivot_i = 1; pivot_i < circuit->pivots.len; pivot_i++) {
    Pivot *pivot = circuit->pivots.array + pivot_i;

    // look for empty connections
    if (pivot->connectionID == 0) {
      // look for wires connected to pivot

      for (u32 wire_i = 1; wire_i < circuit->wires.len; wire_i++) {
        Wire *wire = circuit->wires.array + wire_i;

        // check all 2 pivots
        if (!checkAndGenerateConnection(ctx, pivot_i, wire->pivotID1,
                                        wire->pivotID2)) {
          checkAndGenerateConnection(ctx, pivot_i, wire->pivotID2,
                                     wire->pivotID1);
        }
      }
    }
  }
}

void startSimulation(Ctx *ctx) {
  printf("\nSTARTING SIMULATION\n");
  Circuit *circuit = &ctx->circuit;
  generateConnections(ctx);

  for (u32 pivot_i = 1; pivot_i < circuit->pivots.len; pivot_i++) {
    Pivot *pivot = circuit->pivots.array + pivot_i;

    printf("PIVOT ID: %d, CONNECTION ID: %d\n", pivot_i, pivot->connectionID);
  }
}
void simulate(Ctx *ctx) {}
void stopSimulation(Ctx *ctx) {}
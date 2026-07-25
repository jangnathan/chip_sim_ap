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

void searchAndGenerateConnections(Ctx *ctx, u32 *pivotQueue,
                                  u32 *pivotQueueLen) {

  Circuit *circuit = &ctx->circuit;
  Pivots *pivots = &circuit->pivots;
  Connections *connections = &ctx->connections;

  u32 connectionID = connectionsNew(connections);
  while (*pivotQueueLen > 0) {
    *pivotQueueLen = *pivotQueueLen - 1;
    u32 pivot_i = pivotQueue[*pivotQueueLen];

    pivots->array[pivot_i].connectionID = connectionID;

    for (u32 wire_i = 1; wire_i < circuit->wires.len; wire_i++) {
      Wire *wire = circuit->wires.array + wire_i;

      // add to queue
      if (wire->pivotID1 == pivot_i) {
        if (pivots->array[wire->pivotID2].connectionID == 0) {
          pivotQueue[*pivotQueueLen] = wire->pivotID2;
          *pivotQueueLen = *pivotQueueLen + 1;
        }
      } else if (wire->pivotID2 == pivot_i) {
        if (pivots->array[wire->pivotID1].connectionID == 0) {

          pivotQueue[*pivotQueueLen] = wire->pivotID1;
          *pivotQueueLen = *pivotQueueLen + 1;
        }
      }
    }
  }

  printf("pivotQueueLen after: %d\n", *pivotQueueLen);
}

void generateConnections(Ctx *ctx) {
  Circuit *circuit = &ctx->circuit;
  resetConnections(ctx);

  u32 *pivotQueue = malloc(sizeof(u32) * (circuit->pivots.len - 1));
  u32 pivotQueueLen = 0;

  for (u32 pivot_i = 1; pivot_i < circuit->pivots.len; pivot_i++) {
    Pivot *pivot = circuit->pivots.array + pivot_i;

    // look for empty connections
    if (pivot->connectionID == 0) {
      pivotQueueLen++;
      pivotQueue[0] = pivot_i;

      // do BFS if queue
      searchAndGenerateConnections(ctx, pivotQueue, &pivotQueueLen);
    }
  }

  free(pivotQueue);
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
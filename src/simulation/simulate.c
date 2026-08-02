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
      u32 pivotCEID = pivots->array[pivot_i].ID;
      if (wire->pivotCEID1 == pivotCEID) {
        u32 nextPivotIndex = pivotIndexFromCEID(circuit, wire->pivotCEID2);
        if (pivots->array[nextPivotIndex].connectionID == 0) {
          pivotQueue[*pivotQueueLen] = nextPivotIndex;
          *pivotQueueLen = *pivotQueueLen + 1;
        }
      } else if (wire->pivotCEID2 == pivotCEID) {
        u32 nextPivotIndex = pivotIndexFromCEID(circuit, wire->pivotCEID1);
        if (pivots->array[nextPivotIndex].connectionID == 0) {
          pivotQueue[*pivotQueueLen] = nextPivotIndex;
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

ElectricState *pivotConnectionState(Ctx *ctx, u32 pivotCEID) {
  Circuit *circuit = &ctx->circuit;
  Connections *connections = &ctx->connections;

  u32 pivotIndex = pivotIndexFromCEID(circuit, pivotCEID);
  u32 connectionID = circuit->pivots.array[pivotIndex].connectionID;

  return connections->array + connectionID;
}

u8 simpleChipEvalLogic(SimpleChipType type, u8 a, u8 b) {
  u8 result = 0;

  switch (type) {
  case AND:
    result = (a && b) ? 1 : 0;
    break;
  case OR:
    result = (a || b) ? 1 : 0;
    break;
  case NOT:
    result = a ? 0 : 1;
    break;
  case NAND:
    result = (a && b) ? 0 : 1;
    break;
  case NOR:
    result = (a || b) ? 0 : 1;
    break;
  case XOR:
    result = (a != b) ? 1 : 0;
    break;
  case XNOR:
    result = (a == b) ? 1 : 0;
    break;
  default:
    result = 0;
    break;
  }

  return result;
}

void simulate(Ctx *ctx) {
  Circuit *circuit = &ctx->circuit;
  Connections *connections = &ctx->connections;
  InputChips *inputChips = &circuit->inputChips;

  for (u32 i = 0; i < connections->len; i++) {
    connections->array[i].on_next = 0;
  }

  for (u32 i = 1; i < inputChips->len; i++) {
    InputChip *inputChip = inputChips->array + i;
    ElectricState *electricState =
        pivotConnectionState(ctx, inputChip->pivotCEID_out);

    // cannot disable electricity if there is already electricity if not diode
    if (electricState->on_next == 0) {
      electricState->on_next = inputChip->out;
    }
  }

  u8 stable = 0;
  u16 iteration = 0;
  while (stable == 0) {
    stable = 1;

    SimpleChips *simpleChips = &circuit->simpleChips;
    for (u32 i = 1; i < simpleChips->len; i++) {
      SimpleChip *simpleChip = simpleChips->array + i;
      ElectricState *electricStateA =
          pivotConnectionState(ctx, simpleChip->pivotCEID_A);
      ElectricState *electricStateB =
          pivotConnectionState(ctx, simpleChip->pivotCEID_B);

      simpleChip->out = simpleChipEvalLogic(
          simpleChip->type, electricStateA->on, electricStateB->on);

      ElectricState *electricStateOut =
          pivotConnectionState(ctx, simpleChip->pivotCEID_out);

      if (electricStateOut->on_next == 0) {
        electricStateOut->on_next = simpleChip->out;
      }
    }

    for (u32 i = 0; i < connections->len; i++) {
      ElectricState *electricState = connections->array + i;
      if (electricState->on != electricState->on_next) {
        electricState->on = electricState->on_next;
        stable = 0;
        break;
      }
    }

    iteration++;
    if (iteration > connections->len * 5) {
      break;
    }
  }
}
void stopSimulation(Ctx *ctx) {}
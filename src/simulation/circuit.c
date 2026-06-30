#include "simulation/circuit.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define INIT_ID_SIZE 64

u32 assignCircuitEntity(Circuit *circuit, CircuitEntityType type, u32 typeID) {
  u32 newID = circuit->len;
  circuit->len++;
  u32 circuitsLen = circuit->len;

  if (circuitsLen >= circuit->size) {
    circuit->size *= 2;
    circuit->array =
        realloc(circuit->array, sizeof(CircuitEntity) * circuit->size);
  }

  circuit->array[newID].type = type;
  circuit->array[newID].typeID = typeID;
  return newID;
}

// pivots and wires
void pivotsInit(Pivots *pivots) {
  // 0 is reserved
  pivots->len = 1;
  pivots->size = 256;
  pivots->array = malloc(sizeof(Pivot) * pivots->size);
}
u32 pivotsNew(Circuit *circuit) {
  Pivots *pivots = &circuit->pivots;

  u32 newPivotID = pivots->len;
  pivots->len++;
  u32 pivotsLen = pivots->len;

  if (pivotsLen >= pivots->size) {
    pivots->size *= 2;
    pivots->array = realloc(pivots->array, sizeof(Pivot) * pivots->size);
  }

  u32 newID = assignCircuitEntity(circuit, CE_PIVOT, newPivotID);
  Pivot *pivot = pivots->array + newPivotID;
  pivot->ID = newID;

  return newID;
};

void wiresInit(Wires *wires) {
  // 0 is reserved
  wires->len = 1;
  wires->size = 256;
  wires->array = malloc(sizeof(Wire) * wires->size);
}
u32 wiresNew(Circuit *circuit) {
  Wires *wires = &circuit->wires;

  u32 newWireID = wires->len;
  wires->len++;
  u32 wiresLen = wires->len;

  if (wiresLen >= wires->size) {
    wires->size *= 2;
    wires->array = realloc(wires->array, sizeof(Wire) * wires->size);
  }

  u32 newID = assignCircuitEntity(circuit, CE_WIRE, newWireID);
  Wire *wire = wires->array + newWireID;
  wire->pivotID1 = 0;
  wire->pivotID2 = 0;
  wire->ID = newID;

  return newID;
}

// CHIPS

void simpleChipsInit(SimpleChips *simpleChips) {
  simpleChips->len = 1;
  simpleChips->size = 256;
  simpleChips->array = malloc(sizeof(SimpleChip) * simpleChips->size);
}
void inputChipsInit(InputChips *inputChips) {
  inputChips->len = 1;
  inputChips->size = 256;
  inputChips->array = malloc(sizeof(InputChip) * inputChips->size);
}

u32 simpleChipsNew(Circuit *circuit, SimpleChipOptions *options) {
  SimpleChips *simpleChips = &circuit->simpleChips;

  u32 newChipID = simpleChips->len;
  simpleChips->len++;
  u32 simpleChipsLen = simpleChips->len;

  if (simpleChipsLen >= simpleChips->size) {
    simpleChips->size *= 2;
    simpleChips->array =
        realloc(simpleChips->array, sizeof(SimpleChip) * simpleChips->size);
  }

  u32 newID = assignCircuitEntity(circuit, CE_INPUT, newChipID);
  SimpleChip *simpleChip = simpleChips->array + newChipID;
  simpleChip->type = options->type;
  simpleChip->position = options->position;
  simpleChip->ID = newID;

  return newID;
}
u32 inputChipsNew(Circuit *circuit, InputChipOptions *options) {
  InputChips *inputChips = &circuit->inputChips;

  u32 newChipID = inputChips->len;
  inputChips->len++;
  u32 inputChipsLen = inputChips->len;

  if (inputChipsLen >= inputChips->size) {
    inputChips->size *= 2;
    inputChips->array =
        realloc(inputChips->array, sizeof(InputChip) * inputChips->size);
  }

  u32 newID = assignCircuitEntity(circuit, CE_INPUT, newChipID);
  InputChip *inputChip = inputChips->array + newChipID;
  inputChip->type = options->type;
  inputChip->position = options->position;
  inputChip->out = 0;
  inputChip->ID = newID;

  return newID;
}

void positionCircuitEntity(Circuit *circuit, CircuitEntity *entity, Vec2f pos) {
  switch (entity->type) {
  case CE_PIVOT:
    circuit->pivots.array[entity->typeID].position = pos;
    break;
  case CE_INPUT:
    circuit->inputChips.array[entity->typeID].position = pos;
    break;
  case CE_SIMPLE:
    circuit->simpleChips.array[entity->typeID].position = pos;
  default:
    break;
  }
}

void circuitInit(Circuit *circuit) {
  printf("Circuit initialized");
  circuit->len = 1;
  circuit->size = INIT_ID_SIZE;
  circuit->array = malloc(sizeof(CircuitEntity) * circuit->size);

  pivotsInit(&circuit->pivots);
  wiresInit(&circuit->wires);

  inputChipsInit(&circuit->inputChips);
  simpleChipsInit(&circuit->simpleChips);
}

void circuitFree(Circuit *circuit) {
  free(circuit->array);
  free(circuit->pivots.array);
  free(circuit->wires.array);
  free(circuit->inputChips.array);
  free(circuit->simpleChips.array);
}
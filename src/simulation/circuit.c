#include "simulation/circuit.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define INIT_ID_SIZE 64

void createCircuitEntity(Circuit *circuit, CircuitEntityType type, u32 typeID) {
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

  u32 newID = pivots->len;
  pivots->len++;
  u32 pivotsLen = pivots->len;

  if (pivotsLen >= pivots->size) {
    pivots->size *= 2;
    pivots->array = realloc(pivots->array, sizeof(Pivot) * pivots->size);
  }

  createCircuitEntity(circuit, CE_PIVOT, newID);

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

  u32 newID = wires->len;
  wires->len++;
  u32 wiresLen = wires->len;

  if (wiresLen >= wires->size) {
    wires->size *= 2;
    wires->array = realloc(wires->array, sizeof(Wire) * wires->size);
  }

  createCircuitEntity(circuit, CE_WIRE, newID);

  return newID;
}

// CHIPS

void simpleChipsInit(SimpleChips *simpleChips) {}
u32 simpleChipsNew(Circuit *circuit, const SimpleChipOptions *options) {
  return 0;
}

void inputChipsInit(InputChips *inputChips) {}
u32 inputChipsNew(Circuit *circuit, const InputChipOptions *options) {
  return 0;
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
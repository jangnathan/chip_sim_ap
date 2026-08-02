#include "simulation/circuit.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const char *const SimpleChipsName[] = {"AND", "OR",  "NOT", "NAND",
                                       "NOR", "XOR", "XNOR"};

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

u32 pivotIndexFromCEID(Circuit *circuit, u32 pivotCEID) {
  return circuit->array[pivotCEID].typeID;
}

Pivot *getPivotByCEID(Circuit *circuit, u32 pivotCEID) {
  return &circuit->pivots.array[pivotIndexFromCEID(circuit, pivotCEID)];
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

  // 0 at default
  pivot->designatedChipID = 0;

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
  wire->pivotCEID1 = 0;
  wire->pivotCEID2 = 0;
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

void positionSimpleChip(Circuit *circuit, SimpleChip *simpleChip, Vec2f pos) {
  simpleChip->position = pos;
  getPivotByCEID(circuit, simpleChip->pivotCEID_out)->position =
      translateVec2f(pos, newVec2f(0.0f, 40.0f));

  if (simpleChip->type == NOT) {
    getPivotByCEID(circuit, simpleChip->pivotCEID_A)->position =
        translateVec2f(pos, newVec2f(0.0f, -25.0f));
  } else {
    getPivotByCEID(circuit, simpleChip->pivotCEID_A)->position =
        translateVec2f(pos, newVec2f(-25.0f, -25.0f));
    getPivotByCEID(circuit, simpleChip->pivotCEID_B)->position =
        translateVec2f(pos, newVec2f(25.0f, -25.0f));
  }
}

u32 simpleChipsNew(Circuit *circuit, SimpleChipOptions *options) {
  SimpleChips *simpleChips = &circuit->simpleChips;
  Pivots *pivots = &circuit->pivots;

  u32 newChipID = simpleChips->len;
  simpleChips->len++;
  u32 simpleChipsLen = simpleChips->len;

  if (simpleChipsLen >= simpleChips->size) {
    simpleChips->size *= 2;
    simpleChips->array =
        realloc(simpleChips->array, sizeof(SimpleChip) * simpleChips->size);
  }

  u32 newID = assignCircuitEntity(circuit, CE_SIMPLE, newChipID);
  SimpleChip *simpleChip = simpleChips->array + newChipID;
  simpleChip->type = options->type;
  simpleChip->ID = newID;

  u32 pivotCEID_out = pivotsNew(circuit);
  simpleChip->pivotCEID_out = pivotCEID_out;
  pivots->array[pivotIndexFromCEID(circuit, pivotCEID_out)].designatedChipID = newID;

  u32 pivotCEID_A = pivotsNew(circuit);
  simpleChip->pivotCEID_A = pivotCEID_A;
  pivots->array[pivotIndexFromCEID(circuit, pivotCEID_A)].designatedChipID = newID;

  if (simpleChip->type == NOT) {
    simpleChip->pivotCEID_B = 0;
  } else {
    u32 pivotCEID_B = pivotsNew(circuit);
    simpleChip->pivotCEID_B = pivotCEID_B;
    pivots->array[pivotIndexFromCEID(circuit, pivotCEID_B)].designatedChipID = newID;
  }

  positionSimpleChip(circuit, simpleChip, options->position);

  return newID;
}

Vec2f InputChipPivotOffset() { return newVec2f(0.0f, 60.0f); }

u32 inputChipsNew(Circuit *circuit, InputChipOptions *options) {
  InputChips *inputChips = &circuit->inputChips;
  Pivots *pivots = &circuit->pivots;

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

  u32 pivotCEID = pivotsNew(circuit);
  inputChip->pivotCEID_out = pivotCEID;
  getPivotByCEID(circuit, pivotCEID)->position =
      translateVec2f(options->position, InputChipPivotOffset());
  getPivotByCEID(circuit, pivotCEID)->designatedChipID = newID;

  return newID;
}

void positionCircuitEntity(Circuit *circuit, CircuitEntity *entity, Vec2f pos) {
  switch (entity->type) {
  case CE_PIVOT:
    circuit->pivots.array[entity->typeID].position = pos;
    break;
  case CE_INPUT: {
    InputChip *inputChip = circuit->inputChips.array + entity->typeID;
    inputChip->position = pos;
    getPivotByCEID(circuit, inputChip->pivotCEID_out)->position =
        translateVec2f(pos, InputChipPivotOffset());
    break;
  }
  case CE_SIMPLE: {
    SimpleChip *simpleChip = circuit->simpleChips.array + entity->typeID;
    simpleChip->position = pos;

    positionSimpleChip(circuit, simpleChip, pos);
    break;
  }
  default:
    break;
  }
}

void setPivotCEID_out(Circuit *circuit, CircuitEntity *entity, u32 out) {
  switch (entity->type) {
  case CE_NONE:
    break;
  case CE_SIMPLE: {
    SimpleChip *simpleChip = circuit->simpleChips.array + entity->typeID;
    simpleChip->pivotCEID_out = out;
    break;
  }
  case CE_INPUT: {
    InputChip *inputChip = circuit->inputChips.array + entity->typeID;
    inputChip->pivotCEID_out = out;
    break;
  }
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

void deleteCE(Circuit *circuit, u32 ID) {
  u32 typeID = circuit->array[ID].typeID;
  CircuitEntityType ceType = circuit->array[ID].type;

  u32 replaceCE_ID = 0;

  switch (ceType) {
  case CE_NONE:
    break;
  case CE_PIVOT: {
    u32 designatedChipID = circuit->pivots.array[typeID].designatedChipID;
    // throw an error if it has a designated chipID
    if (designatedChipID > 0) {
      return 1; // No error enum for now
    }

    // delete connected wires
    // what if multiple wires connected to same pivot
    for (u32 i = 1; i < circuit->wires.len; i++) {
      Wire *wire = circuit->wires.array + i;
      if (wire->pivotCEID1 == ID || wire->pivotCEID2 == ID) {
        deleteCE(circuit, wire->ID);
        i--;
      }
    }

    circuit->pivots.len--;

    // typeID now points to replaced
    circuit->pivots.array[typeID] = circuit->pivots.array[circuit->pivots.len];
    replaceCE_ID = circuit->pivots.array[typeID].ID;
    break;
  }
  case CE_WIRE: {
    circuit->wires.len--;

    circuit->wires.array[typeID] = circuit->wires.array[circuit->wires.len];

    replaceCE_ID = circuit->pivots.array[typeID].ID;
    break;
  }
  case CE_INPUT: {
    circuit->inputChips.len--;

    // delete assigned pivot
    u32 pivotCEID = circuit->inputChips.array[typeID].pivotCEID_out;
    u32 pivotTypeID = pivotIndexFromCEID(circuit, pivotCEID);

    // set the pivot to be lone
    circuit->pivots.array[pivotTypeID].designatedChipID = 0;
    deleteCE(circuit, pivotCEID);
    // typeID now points to replaced
    circuit->inputChips.array[typeID] =
        circuit->inputChips.array[circuit->inputChips.len];

    replaceCE_ID = circuit->pivots.array[typeID].ID;

    break;
  }
  case CE_SIMPLE: {
    circuit->simpleChips.len--;

    // delete assigned pivots
    u32 pivotCEID_out = circuit->simpleChips.array[typeID].pivotCEID_out;
    u32 pivotTypeID_out = pivotIndexFromCEID(circuit, pivotCEID_out);
    // set the pivot to be lone
    circuit->pivots.array[pivotTypeID_out].designatedChipID = 0;
    deleteCE(circuit, pivotCEID_out);

    u32 pivotCEID_A = circuit->simpleChips.array[typeID].pivotCEID_A;
    u32 pivotTypeID_A = pivotIndexFromCEID(circuit, pivotCEID_A);
    // set the pivot to be lone
    circuit->pivots.array[pivotTypeID_A].designatedChipID = 0;
    deleteCE(circuit, pivotCEID_A);

    u32 pivotCEID_B = circuit->simpleChips.array[typeID].pivotCEID_B;
    if (pivotCEID_B != 0) { // NOT gates only have pivotCEID_A
      u32 pivotTypeID_B = pivotIndexFromCEID(circuit, pivotCEID_B);
      // set the pivot to be lone
      circuit->pivots.array[pivotTypeID_B].designatedChipID = 0;
      deleteCE(circuit, pivotCEID_B);
    }

    // typeID now points to replaced
    circuit->simpleChips.array[typeID] =
        circuit->simpleChips.array[circuit->simpleChips.len];


    replaceCE_ID = circuit->pivots.array[typeID].ID;
    break;
  }
  default:
    break;
  }

  // do nothing, to avoid errors where wires point to old pivots
  // circuit->len--;
  // ID now points to replaced
  //circuit->array[ID] = circuit->array[circuit->len];
  circuit->array[replaceCE_ID].typeID = typeID;
  return;
}
// "Chips" include clocks, etc
#pragma once
#include "constants.h"
#include "unit.h"

// PIVOT
typedef struct {
  Vec2f position;
  u32 ID;
  u32 connectionID;

  // for chip terminals
  u32 designatedChipID;
} Pivot;

typedef struct {
  Pivot *array;
  u32 len;
  u32 size;
} Pivots;

// WIRE
typedef struct {
  u32 ID;
  u32 pivotID1;
  u32 pivotID2;
} Wire;

typedef struct {
  Wire *array;
  u32 len;
  u32 size;
} Wires;

// SIMPLE CHIP

// Usually 8 bits are enough for chips
// E.g. make 4 way 1 bit mux e.g. 2 bits for selection + 4 bits for in + 1 bit
// for out = 7 use bit shifting and AND to process individual bits

typedef enum {
  AND,
  OR,
  NOT,
  NAND,
  NOR,
  XOR,
  XNOR,
} SimpleChipType;

typedef struct {
  Vec2f position;

  SimpleChipType type;
  u32 ID;

  u32 pivotID_A;
  u32 pivotID_B;
  u32 pivotID_out;

  u8 nextOut; // to synchronize
  u8 out;
  // it can support up to 8 bits of output
} SimpleChip;

typedef struct {
  SimpleChip *array;
  u32 len;
  u32 size;
} SimpleChips;

// INPUT CHIP
typedef enum { CLOCK, SWITCH } InputChipType;

typedef struct {
  Vec2f position;

  InputChipType type;
  u32 ID;

  u32 pivotID_out;

  u8 out;
} InputChip;

typedef struct {
  InputChip *array;
  u32 len;
  u32 size;
} InputChips;

// Chip entity

typedef enum {
  CE_NONE,
  CE_PIVOT,
  CE_WIRE,
  CE_SIMPLE,
  CE_INPUT,
} CircuitEntityType;

typedef struct {
  u32 typeID;

  CircuitEntityType type;
} CircuitEntity;

typedef struct {
  Wires wires;
  Pivots pivots;

  SimpleChips simpleChips;
  InputChips inputChips;

  u32 len;
  u32 size;
  CircuitEntity *array;
} Circuit;

void positionCircuitEntity(Circuit *circuit, CircuitEntity *CircuitEntity,
                           Vec2f pos);

u32 pivotsNew(Circuit *circuit);

u32 wiresNew(Circuit *circuit);

typedef struct {
  SimpleChipType type;
  Vec2f position;
} SimpleChipOptions;

u32 simpleChipsNew(Circuit *circuit, SimpleChipOptions *options);

typedef struct {
  InputChipType type;
  Vec2f position;
} InputChipOptions;

u32 inputChipsNew(Circuit *circuit, InputChipOptions *options);

void circuitInit(Circuit *circuit);
void circuitFree(Circuit *circuit);

void deleteCE(Circuit *circuit, u32 ID);
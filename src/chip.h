// "Chips" include clocks, etc
#pragma once
#include "constants.h"

// Usually 8 bits are enough for chips
// E.g. make 4 way 1 bit mux e.g. 2 bits for selection + 4 bits for in + 1 bit for out = 7
// use bit shifting and AND to process individual bits

typedef enum {
	AND,
	OR,
	NOT,
	NAND,
	NOR,
	XOR,
	XNOR,
} SimpleChipType;

typedef enum {
	CLOCK,
	SWITCH
} InputType;

typedef enum {
	CE_SIMPLE,
	CE_INPUT
} ChipEntityType;

// Input wiring
typedef struct {
	u32 ID;
	u8 n; // if a chip has multiple outputs (e.g. dmux), then decide which one
	// default = 0
} InSignal;

typedef struct {
	u32 ID;

	// inputs
	InSignal aID;
	InSignal bID;

	u8 out;
	// it can support up to 8 bits of output
} SimpleChip;

typedef struct {
	u32 ID;
	ChipEntityType type;
} ChipEntity;

typedef struct {
	u32 len;
	u32 size;

	u32 simpleChipsLen;
	u32 simpleChipsSize;
	SimpleChip *simpleChipsArray;
	ChipEntity *array;
} Chips;

void chipsInit(Chips *chips);

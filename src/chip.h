// "Chips" include clocks, etc
#pragma once
#include "constants.h"
#include "unit.h"

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
} InputChipType;

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
	SimpleChipType type;

	// inputs
	InSignal inSignals[2];

	u8 out;
	// it can support up to 8 bits of output
} SimpleChip;

typedef struct {
	u32 ID;
	InputChipType type;
	u8 out;
} InputChip;

typedef struct {
	u32 ID;
	// positions to render
	Vec2 position;

	ChipEntityType type;
} ChipEntity;

typedef struct {
	u32 len;
	u32 size;

	u32 simpleChipsLen;
	u32 simpleChipsSize;

	u32 inputChipsLen;
	u32 inputChipsSize;

	InputChip *inputChipsArray;
	SimpleChip *simpleChipsArray;
	ChipEntity *array;
} Chips;

void chipsInit(Chips *chips);
u32 newSimpleChip(Chips *chips, SimpleChipType type);
u32 newInputChip(Chips *chips, InputChipType type);
u8 linkChipInsignal(Chips *chips, u32 targetID, u8 targetN, u32 inputID, u8 n);
u8 getInSignalOutput(Chips *chips, InSignal inSignal);

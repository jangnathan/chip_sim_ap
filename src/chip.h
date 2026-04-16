// "Chips" include clocks, etc
#pragma once
#include "constants.h"

// Usually 8 bits are enough for chips
// E.g. make 4 way 1 bit mux e.g. 2 bits for selection + 4 bits for in + 1 bit for out = 7
// use bit shifting and AND to process individual bits

typedef enum {
	// two in. one out
	AND,
	OR,
	NOT,
	NAND,
	NOR,
	XOR,
	XNOR,

	// one out
	CLOCK,

	// end of 2x8
} ChipType;

// Input wiring
typedef struct {
	u32 ID;
	u8 n; // if a chip has multiple outputs (e.g. dmux), then decide which one
	// default = 0
} Input;

typedef struct {
	// inputs
	u32 aID;
	u32 bID;

	u8 out;
	// it can support up to 8 bits of output
} Chip2x8;

typedef struct {
	u32 ID;
	ChipType type;
} Chip;

typedef struct {
	u32 len;
	u32 size;
	Chip *array;

	u32 chip2x8sLen;
	u32 chip2x8sSize;
	Chip2x8 *chip2x8sArray;
} Chips;

void chipsInit(Chips *chips);

#include "chip.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define INIT_ID_SIZE 64

void chipsInit(Chips *chips) {
	// set len to 1 to reserve 0 for error handling
	chips->len = 1;
	chips->size = INIT_ID_SIZE;
	chips->array = malloc(sizeof(ChipEntity) * chips->size);
	chips->array[0].position = newVec2(0.0f, 0.0f);

	chips->simpleChipsLen = 1;
	chips->simpleChipsSize = 16;
	chips->simpleChipsArray = malloc(sizeof(SimpleChip) * chips->simpleChipsSize);

	chips->inputChipsLen = 1;
	chips->inputChipsSize = 16;
	chips->inputChipsArray = malloc(sizeof(InputChip) * chips->inputChipsSize);
}

u32 newChipEntity(Chips *chips, u32 ID, ChipEntityType type) {
	if (chips->len >= chips->size) {
		if (chips->size == UINT32_MAX) {
			fprintf(stderr, "Cannot allocate any more memory for chips");
			return 0;
		}

		chips->size *= 2;
		chips->array = realloc(chips->array, sizeof(ChipEntity) * chips->size);
	}

	chips->array[chips->len].parentID = 0;
	chips->array[chips->len].typeID = ID;
	chips->array[chips->len].type = type;
	chips->len++;
	return chips->len - 1;
}

u32 newSimpleChip(Chips *chips, SimpleChipType type) {
	if (chips->simpleChipsLen >= chips->simpleChipsSize) {
		if (chips->simpleChipsSize == UINT32_MAX) {
			fprintf(stderr, "Cannot allocate any more memory for simple chips");
			return 0;
		}

		chips->simpleChipsSize *= 2;
		chips->simpleChipsArray = realloc(chips->simpleChipsArray, sizeof(SimpleChip) * chips->simpleChipsSize);
	}

	chips->simpleChipsArray[chips->simpleChipsLen].type = type;
	chips->simpleChipsArray[chips->simpleChipsLen].ID = chips->len;

	chips->array[chips->len].typeID = chips->simpleChipsLen;
	chips->array[chips->len].type = CE_SIMPLE;

	chips->simpleChipsLen++;
	return newChipEntity(chips, chips->simpleChipsLen - 1, CE_SIMPLE);
}

u32 newInputChip(Chips *chips, InputChipType type) {
	if (chips->inputChipsLen >= chips->inputChipsSize) {
		if (chips->inputChipsSize == UINT32_MAX) {
			fprintf(stderr, "Cannot allocate any more memory for input chips");
			return 0;
		}

		chips->inputChipsSize *= 2;
		chips->inputChipsArray = realloc(chips->inputChipsArray, sizeof(SimpleChip) * chips->inputChipsSize);
	}

	chips->inputChipsArray[chips->inputChipsLen].out = 0;
	chips->inputChipsArray[chips->inputChipsLen].type = type;
	chips->inputChipsArray[chips->inputChipsLen].ID = chips->len;
	chips->inputChipsLen++;
	return newChipEntity(chips, chips->inputChipsLen - 1, CE_INPUT);
}

// link chip's Nth input to another chip's Nth output (which is current chip's input)
u8 linkChipInsignal(Chips *chips, u32 targetID, u8 targetN, u32 inputID, u8 n) {
	ChipEntity chipEntity = chips->array[targetID];
	switch (chipEntity.type) {
		case CE_NONE:
			fprintf(stderr, "Cannot link insignal to NONE");
			return 0;
			break;
		case CE_SIMPLE:
			if (targetN > 2) {
				fprintf(stderr, "Cannot link insignal to input");
				return 0;
			}
			SimpleChip *chip = &chips->simpleChipsArray[chipEntity.typeID];

			chip->inSignals[targetN].ID = inputID;
			chip->inSignals[targetN].n = n;
			break;
		case CE_INPUT:
			fprintf(stderr, "Cannot link insignal to input");
			return 0;
	}
	return 1;
}

u8 getInSignalOutput(Chips *chips, InSignal inSignal) {
	ChipEntity chipEntity = chips->array[inSignal.ID];
	u32 typeID = chipEntity.typeID;
	switch (chipEntity.type) {
		case CE_NONE:
			break;
		case CE_SIMPLE:
			return chips->simpleChipsArray[typeID].out;
			break;
		case CE_INPUT:
			return chips->inputChipsArray[typeID].out;
			break;
	}

	return 0;
	// implement this function right here
}

ChipEntity *getChip(Chips *chips, u32 ID) {
	return chips->array + ID;
}
SimpleChip *getSimpleChip(Chips *chips, u32 ID) {
	if (chips->array[ID].type != CE_SIMPLE) {
		fprintf(stderr, "Chip is not a simple chip");
		exit(1);
	}
	return chips->simpleChipsArray + chips->array[ID].typeID;
}
InputChip *getInputChip(Chips *chips, u32 ID) {
	if (chips->array[ID].type != CE_INPUT) {
		fprintf(stderr, "Chip is not a input chip");
		exit(1);
	}
	return chips->inputChipsArray + chips->array[ID].typeID;
}

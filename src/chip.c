#include "chip.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define INIT_ID_SIZE 64

void chipsInit(Chips *chips) {
	chips->len = 0;
	chips->size = INIT_ID_SIZE;
	chips->array = malloc(sizeof(ChipEntity) * chips->size);

	chips->simpleChipsLen = 0;
	chips->simpleChipsSize = 16;
	chips->simpleChipsArray = malloc(sizeof(SimpleChip) * chips->simpleChipsSize);

	chips->inputChipsLen = 0;
	chips->inputChipsSize = 16;
	chips->inputChipsArray = malloc(sizeof(InputChip) * chips->inputChipsSize);
}

u32 newChipEntity(Chips *chips, u32 ID, ChipEntityType type) {
	if (chips->len >= chips->size) {
		if (chips->size == UINT32_MAX) {
			fprintf(stderr, "Cannot allocate any more memory for chips");
			exit(1);
		}

		chips->size *= 2;
		chips->array = realloc(chips->array, sizeof(ChipEntity) * chips->size);
	}

	chips->array[chips->len].ID = ID;
	chips->array[chips->len].type = type;
	chips->len++;
	return chips->len - 1;
}

u32 newSimpleChip(Chips *chips, SimpleChipType type) {
	if (chips->simpleChipsLen >= chips->simpleChipsSize) {
		if (chips->simpleChipsSize == UINT32_MAX) {
			fprintf(stderr, "Cannot allocate any more memory for simple chips");
			exit(1);
		}

		chips->simpleChipsSize *= 2;
		chips->simpleChipsArray = realloc(chips->simpleChipsArray, sizeof(SimpleChip) * chips->simpleChipsSize);
	}

	chips->simpleChipsArray[chips->simpleChipsLen].type = type;

	chips->array[chips->len].ID = chips->simpleChipsLen;
	chips->array[chips->len].type = CE_SIMPLE;

	chips->simpleChipsLen++;
	return newChipEntity(chips, chips->simpleChipsLen - 1, CE_SIMPLE);
}

u32 newInputChip(Chips *chips, InputChipType type) {
	if (chips->inputChipsLen >= chips->inputChipsSize) {
		if (chips->inputChipsSize == UINT32_MAX) {
			fprintf(stderr, "Cannot allocate any more memory for input chips");
			exit(1);
		}

		chips->inputChipsSize *= 2;
		chips->inputChipsArray = realloc(chips->inputChipsArray, sizeof(SimpleChip) * chips->inputChipsSize);
	}

	chips->inputChipsArray[chips->inputChipsLen].type = type;
	chips->inputChipsLen++;
	return newChipEntity(chips, chips->inputChipsLen - 1, CE_INPUT);
}

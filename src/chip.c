#include "chip.h"
#include <stdlib.h>

#define INIT_ID_SIZE 64

void chipsInit(Chips *chips) {
	chips->len = 0;
	chips->size = INIT_ID_SIZE;
	chips->array = malloc(sizeof(ChipEntity) * chips->size);

	chips->simpleChipsLen = 0;
	chips->simpleChipsSize = 16;
	chips->simpleChipsArray = malloc(sizeof(SimpleChip) * chips->simpleChipsSize);
}

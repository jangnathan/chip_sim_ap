#include "chip.h"
#include <stdlib.h>

#define INIT_ID_SIZE 64

// where to look for in a arrays
enum ChipMemType {
	CHIP_2X8
};

enum ChipMemType getMemType(ChipType type) {
	if (type <= CLOCK) {
		return CHIP_2X8;
	}
	return 0;
};

void chipsInit(Chips *chips) {
	chips->len = 0;
	chips->size = INIT_ID_SIZE;
	chips->array = malloc(sizeof(Chip) * chips->size);

	chips->chip2x8sLen = 0;
	chips->chip2x8sSize = 16;
	chips->chip2x8sArray = malloc(sizeof(Chip2x8) * chips->chip2x8sSize);
}

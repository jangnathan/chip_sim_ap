#include "simulate.h"

u8 simulateSimpleChip(Chips *chips, SimpleChip *simpleChip) {
	u8 aOut = getInSignalOutput(chips, simpleChip->inSignals[0]);
	u8 bOut = getInSignalOutput(chips, simpleChip->inSignals[1]);
	
	switch (simpleChip->type) {
		case AND:
			simpleChip->out = aOut && bOut;
			break;
		case OR:
			simpleChip->out = aOut || bOut;
			break;
		case NOT:
			// ignore bOut for this
			simpleChip->out = !aOut;
			break;
		case NAND:
			simpleChip->out = !(aOut && bOut);
			break;
		case NOR:
			simpleChip->out = !(aOut || bOut);
			break;
		case XOR:
			simpleChip->out = aOut != bOut;
			break;
		case XNOR:
			simpleChip->out = aOut == bOut;
			break;
	}

	return 1;
}

u8 simulate(Chips *chips) {
	for (u32 i = 0; i < chips->len; i++) {
		ChipEntity chip = chips->array[i];
		switch (chip.type) {
			case CE_SIMPLE:
				simulateSimpleChip(chips, chips->simpleChipsArray + chip.ID);
				break;
			case CE_INPUT:
				// no need to simulate input
				break;
		}
	}
	return 1;
}

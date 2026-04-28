#include "simulate.h"
#include <stdio.h>

u8 simulateSimpleChip(Chips *chips, SimpleChip *simpleChip) {
	u8 aOut = getInSignalOutput(chips, simpleChip->inSignals[0]);
	u8 bOut = getInSignalOutput(chips, simpleChip->inSignals[1]);
	
	switch (simpleChip->type) {
		case AND:
			simpleChip->nextOut = aOut & bOut;
			break;
		case OR:
			simpleChip->nextOut = aOut | bOut;
			break;
		case NOT:
			// ignore bOut for this
			simpleChip->nextOut = !aOut;
			break;
		case NAND:
			simpleChip->nextOut = !(aOut & bOut);
			break;
		case NOR:
			simpleChip->nextOut = !(aOut | bOut);
			break;
		case XOR:
			simpleChip->nextOut = aOut != bOut;
			break;
		case XNOR:
			simpleChip->nextOut = aOut == bOut;
			break;
	}

	return 1;
}

u8 deltaCycle(Chips *chips) {
	u16 iter = 0;
	u8 stable = 0;

	while (stable == 0) {
		stable = 1;
		for (u32 i = 0; i < chips->simpleChipsLen; i++) {
			simulateSimpleChip(chips, chips->simpleChipsArray + i);
		}
		for (u32 i = 0; i < chips->simpleChipsLen; i++) {
			SimpleChip *simpleChip = chips->simpleChipsArray + i;
			if (simpleChip->out != simpleChip->nextOut) {
				simpleChip->out = simpleChip->nextOut;
				stable = 0;
			}
		}

		iter++;
		if (iter > 5000) {
			printf("Race condition\n");
			break;
		}
	}
}

u8 simulate(Chips *chips) {
	deltaCycle(chips);
	
	// we dont simulate input chips
	return 1;
}

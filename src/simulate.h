#pragma once
#include "constants.h"
#include "chip.h"

// Can Make a clone of all chips (hard to manage)
// Have every chip use (t - 1) and (t) signal?
// Store signals of every chip in an array (hacky)

u8 simulate(Chips *chips);

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "chip.h"

typedef struct {
	Chips chips;
} Ctx;

void initCtx(Ctx *ctx) {
	chipsInit(&ctx->chips);
}

int main() {
	Ctx ctx;
	initCtx(&ctx);

	printf("hello world!");
}

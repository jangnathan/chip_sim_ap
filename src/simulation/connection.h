#pragma once
#include "constants.h"

typedef struct {
    u8 on;
    u8 on_next;
} ElectricState;

typedef struct {
    ElectricState *array;
    u32 len;
    u32 size;
} Connections;

void connectionsInit(Connections *connections);
u32 connectionsNew(Connections *connections);
void connectionsFree(Connections *connections);
#include "connection.h"
#include "circuit.h"
#include <stdlib.h>

void connectionsInit(Connections *connections) {
    connections->len = 1; // 0 is reserved
    connections->size = 64;
    connections->array = malloc(sizeof(ElectricState) * connections->size);
}
u32 connectionsNew(Connections *connections) {
    u32 connectionID = connections->len;
    connections->len++;
    if (connections->len >= connections->size) {
        connections->size *= 2;
        connections->array = realloc(connections->array, sizeof(ElectricState) * connections->size);
    }

    ElectricState *electricState = connections->array + connectionID;
    *electricState = 0;

    return connectionID;
}
void connectionsFree(Connections *connections) {
    free(connections->array);
}

#include "serial/serialize.h"
#include "serial/file_io.h"

#include <string.h>

static void serializeVec2f(BinaryWriter *writer, Vec2f value) {
  u32 xBits = 0;
  u32 yBits = 0;

  memcpy(&xBits, &value.x, sizeof(value.x));
  memcpy(&yBits, &value.y, sizeof(value.y));

  BinaryWriter_writeU32(writer, xBits);
  BinaryWriter_writeU32(writer, yBits);
}

void serializePivot(BinaryWriter *writer, Pivot *pivot) {
  serializeVec2f(writer, pivot->position);
  BinaryWriter_writeU32(writer, pivot->ID);
  BinaryWriter_writeU32(writer, pivot->connectionID);
  BinaryWriter_writeU32(writer, pivot->designatedChipID);
}

void serializeWire(BinaryWriter *writer, Wire *wire) {
  BinaryWriter_writeU32(writer, wire->ID);
  BinaryWriter_writeU32(writer, wire->pivotCEID1);
  BinaryWriter_writeU32(writer, wire->pivotCEID2);
}

void serializeSimpleChip(BinaryWriter *writer, SimpleChip *chip) {
  serializeVec2f(writer, chip->position);
  BinaryWriter_writeU32(writer, (u32)chip->type);
  BinaryWriter_writeU32(writer, chip->ID);
  BinaryWriter_writeU32(writer, chip->pivotCEID_A);
  BinaryWriter_writeU32(writer, chip->pivotCEID_B);
  BinaryWriter_writeU32(writer, chip->pivotCEID_out);
  BinaryWriter_writeByte(writer, chip->nextOut);
  BinaryWriter_writeByte(writer, chip->out);
}

void serializeInputChip(BinaryWriter *writer, InputChip *chip) {
  serializeVec2f(writer, chip->position);
  BinaryWriter_writeU32(writer, (u32)chip->type);
  BinaryWriter_writeU32(writer, chip->ID);
  BinaryWriter_writeU32(writer, chip->pivotCEID_out);
  BinaryWriter_writeByte(writer, chip->out);
}

void serializeCircuitEntity(BinaryWriter *writer, CircuitEntity *entity) {
  BinaryWriter_writeU32(writer, entity->typeID);
  BinaryWriter_writeU32(writer, (u32)entity->type);
}

void serializeCircuit(BinaryWriter *writer, Circuit *circuit) {
  // PIVOTS
  if (circuit->pivots.len > 1) {
    BinaryWriter_writeBuf(writer, "PIVOT", 5);
    BinaryWriter_writeU32(writer, circuit->pivots.len - 1);
    for (u32 i = 1; i < circuit->pivots.len; i++) {
      serializePivot(writer, circuit->pivots.array + i);
    }
  }

  // WIRES
  if (circuit->wires.len > 1) {
    BinaryWriter_writeBuf(writer, "WIRES", 5);
    BinaryWriter_writeU32(writer, circuit->wires.len - 1);

    for (u32 i = 1; i < circuit->wires.len; i++) {
      serializeWire(writer, circuit->wires.array + i);
    }
  }

  // SIMPLE CHIPS
  if (circuit->simpleChips.len > 1) {
    BinaryWriter_writeBuf(writer, "SIMPL", 5);
    BinaryWriter_writeU32(writer, circuit->simpleChips.len - 1);

    for (u32 i = 1; i < circuit->simpleChips.len; i++) {
      serializeSimpleChip(writer, circuit->simpleChips.array + i);
    }
  }

  // INPUT CHIPS
  if (circuit->inputChips.len > 1) {
    BinaryWriter_writeBuf(writer, "INPUT", 5);
    BinaryWriter_writeU32(writer, circuit->inputChips.len - 1);

    for (u32 i = 1; i < circuit->inputChips.len; i++) {
      serializeInputChip(writer, circuit->inputChips.array + i);
    }
  }

  // CIRCUIT ENTITY
  if (circuit->len > 1) {
    BinaryWriter_writeBuf(writer, "CIRCU", 5);
    BinaryWriter_writeU32(writer, circuit->len - 1);

    for (u32 i = 1; i < circuit->len; i++) {
        serializeCircuitEntity(writer, circuit->array + i);
    }
  }
}

void serializeCtx(Ctx *ctx, FILE *fptr) {
  BinaryWriter writer;
  writer.fptr = fptr;
  writer.order = BYTE_ORDER_LITTLE_ENDIAN;
  BinaryWriter_init(&writer);

  char signature[] = "chipsim_";
  BinaryWriter_writeBuf(&writer, signature, 8);

  // write version
  BinaryWriter_writeU16(&writer, 0);

  // write sim name
  u8 paddingBytesNeeded = 0;
  for (u8 i = 0; i < sizeof(ctx->name); i++) {
    if (ctx->name[i] == '\0') {
      paddingBytesNeeded = sizeof(ctx->name) - i;
      break;
    }
    BinaryWriter_writeByte(&writer, ctx->name[i]);
  }
  BinaryWriter_padding(&writer, paddingBytesNeeded);

  serializeCircuit(&writer, &ctx->circuit);

  BinaryWriter_flush(&writer);
}
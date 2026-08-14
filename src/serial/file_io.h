#include <stdio.h>
#include "constants.h"

#define FILE_BUF_SIZE_MAX 4096

typedef enum {
    BYTE_ORDER_LITTLE_ENDIAN,
    BYTE_ORDER_BIG_ENDIAN
} ByteOrder;

typedef struct {
    u8 buffer[FILE_BUF_SIZE_MAX];
    FILE *fptr;
    u32 position;
    u32 bufferPos;
    u32 bufferSize;

    ByteOrder order;
} BinaryReader;

void BinaryReader_init(BinaryReader *reader);

u8 BinaryReader_readByte(BinaryReader *reader);
u16 BinaryReader_readU16(BinaryReader *reader);
u32 BinaryReader_readU32(BinaryReader *reader);
u8 BinaryReader_readToBuf(BinaryReader *reader, u8 *buf, u32 size);

typedef struct {
    u8 buffer[FILE_BUF_SIZE_MAX];
    FILE *fptr;
    u32 position;
    u32 bufferPos;

    ByteOrder order;
} BinaryWriter;

void BinaryWriter_init(BinaryWriter *writer);
void BinaryWriter_writeByte(BinaryWriter *writer, u8 byte);
void BinaryWriter_writeU16(BinaryWriter *writer, u16 num);
void BinaryWriter_writeU32(BinaryWriter *writer, u32 num);
void BinaryWriter_writeBuf(BinaryWriter *writer, u8 *buf, u32 size);
void BinaryWriter_flush(BinaryWriter *writer);
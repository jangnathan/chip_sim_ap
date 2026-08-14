#include "file_io.h"

void BinaryReader_init(BinaryReader *reader) {
  reader->position = 0;
  reader->bufferPos = 0;
  reader->bufferSize = 0;

  if (sizeof(reader->buffer) < 256) {
    printf("Buffer size should be over 256");
  }
}

u8 BinaryReader_readByte(BinaryReader *reader) {
  // load buffer
  if (reader->bufferPos >= reader->bufferSize) {
    u32 bytesRead = 0;

    bytesRead = fread(reader->buffer, 1, sizeof(reader->buffer), reader->fptr);
    if (bytesRead == 0) {
      return 0;
    }

    reader->bufferSize = bytesRead; // account for EOF
    reader->bufferPos = 0;
  }

  reader->bufferPos++;
  reader->position++;

  return reader->buffer[reader->bufferPos];
}

u8 readBytesSmall(BinaryReader *reader, u8 *buf, u8 size) {
  reader->position += size;

  if (reader->bufferPos + size > reader->bufferSize) {
    u8 bytesBeforeOver = reader->bufferSize - reader->bufferPos;

    for (u8 i = 0; i < bytesBeforeOver; i++) {
      buf[i] = reader->buffer[reader->bufferPos + i];
    }

    u32 bytesRead = 0;
    bytesRead = fread(reader->buffer, 1, sizeof(reader->buffer), reader->fptr);

    if (bytesRead < size - bytesBeforeOver) {
      return 0;
    }

    for (u8 i = 0; i < size - bytesBeforeOver; i++) {
      buf[bytesBeforeOver + i] = reader->buffer[i];
    }
    reader->bufferPos = size - bytesBeforeOver;
    return 1;
  }

  for (u8 i = 0; i < size; i++) {
    buf[i] = reader->buffer[reader->bufferPos + i];
  }
  reader->bufferPos += size;
  return 1;
}

void setEndianness(u8 *buf, ByteOrder byteOrder, u8 size) {
  // swap condition
  if (MACHINE_IS_LITTLE_ENDIAN != (byteOrder == BYTE_ORDER_LITTLE_ENDIAN)) {
    u8 temp = 0;
    for (u8 i = 0; i < size / 2; i++) {
      temp = buf[i];
      buf[i] = buf[size - i - 1];
      buf[size - i - 1] = temp;
    }
  }
}

u16 BinaryReader_readU16(BinaryReader *reader) {
  u8 buf[2];
  if (!readBytesSmall(reader, buf, 2)) {
    return 0;
  }

  setEndianness(buf, reader->order, 2);
  u16 num = buf[0] + (buf[1] << 8);
  return num;
}

u32 BinaryReader_readU32(BinaryReader *reader) {
  u8 buf[4];
  if (!readBytesSmall(reader, buf, 4)) {
    return 0;
  }

  setEndianness(buf, reader->order, 4);
  u32 num = buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
  return num;
}

void BinaryWriter_init(BinaryWriter *writer) {
  writer->bufferPos = 0;
  writer->position = 0;
}

void BinaryWriter_writeByte(BinaryWriter *writer, u8 byte) {
  if (writer->bufferPos >= sizeof(writer->buffer)) {
    fwrite(writer->buffer, 1, sizeof(writer->buffer), writer->fptr);
    writer->bufferPos = 0;
  }

  writer->buffer[writer->bufferPos] = byte;

  writer->bufferPos++;
  writer->position++;
}
u8 writeBufferSmall(BinaryWriter *writer, u8 *buf, u32 size) {
  writer->position += size;

  if (writer->bufferPos + size > sizeof(writer->buffer)) {
    u32 bytesBeforeOver = sizeof(writer->buffer) - writer->bufferPos;

    for (u32 i = 0; i < bytesBeforeOver; i++) {
      writer->buffer[writer->bufferPos + i] = buf[i];
    }

    // flush buffer
    fwrite(writer->buffer, 1, sizeof(writer->buffer), writer->fptr);

    for (u32 i = 0; i < size - bytesBeforeOver; i++) {
      writer->buffer[i] = buf[i + bytesBeforeOver];
    }

    writer->bufferPos = size - bytesBeforeOver;
    return 1;
  }

  for (u32 i = 0; i < size; i++) {
    writer->buffer[i + writer->bufferPos] = buf[i];
  }

  return 1;
}
void BinaryWriter_writeU16(BinaryWriter *writer, u16 num) {
  u8 buf[2] = {num & 0xFF, num >> 8};
  setEndianness(buf, writer->order, 2);

  writeBufferSmall(writer, buf, 2);
}
void BinaryWriter_writeU32(BinaryWriter *writer, u32 num) {
  u8 buf[4] = {num & 0xFF, (num >> 8) & 0xFF, (num >> 16) & 0xFF,
	       (num >> 24) & 0xFF};
  setEndianness(buf, writer->order, 4);

  writeBufferSmall(writer, buf, 4);
}
void BinaryWriter_writeBuf(BinaryWriter *writer, u8 *buf, u32 size);

void BinaryWriter_flush(BinaryWriter *writer) {
  fwrite(writer->buffer, 1, writer->bufferPos, writer->fptr);
  writer->bufferPos = 0;
}
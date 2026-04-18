#ifndef MEMSCANNER_H
#define MEMSCANNER_H

#include "STD_TYPES.h"

/* 1. Read a single byte from memory [cite: 23] */
uint8 MemScanner_ReadByte(ConstVoidPtr baseAddr, uint32 offset);

/* 2. Read 16-bit (Little-Endian) [cite: 25] */
uint16 MemScanner_ReadHalfWord(ConstVoidPtr baseAddr, uint32 offset);

/* 3. Read 32-bit (Little-Endian) [cite: 26] */
uint32 MemScanner_ReadWord(ConstVoidPtr baseAddr, uint32 offset);

/* 4. Write a single byte to memory [cite: 27] */
void MemScanner_WriteByte(VoidPtr baseAddr, uint32 offset, uint8 value);

/* 5. Print memory as formatted hex [cite: 28] */
void MemScanner_HexDump(ConstVoidPtr baseAddr, uint32 size);

/* 6. Fill memory with a value [cite: 30] */
void MemScanner_MemFill(VoidPtr baseAddr, uint32 size, uint8 value);

/* 7. Compare two memory regions [cite: 32] */
sint32 MemScanner_MemCompare(ConstVoidPtr addr1, VoidPtr addr2, uint32 size);

/* 8. Find first occurrence of a byte [cite: 34] */
sint32 MemScanner_FindByte(ConstVoidPtr baseAddr, uint32 size, uint8 target);

#endif
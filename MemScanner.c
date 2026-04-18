#include <stdio.h>
#include "MemScanner.h"

/* -----------------------------------------------------------------------
 * 1. Read a single byte
 * ----------------------------------------------------------------------- */
uint8 MemScanner_ReadByte(ConstVoidPtr baseAddr, uint32 offset)
{
    const uint8 *base = (const uint8 *)baseAddr;
    return base[offset];
}

/* -----------------------------------------------------------------------
 * 2. Read 16-bit half-word (Little-Endian)
 * ----------------------------------------------------------------------- */
uint16 MemScanner_ReadHalfWord(ConstVoidPtr baseAddr, uint32 offset)
{
    const uint8 *base = (const uint8 *)baseAddr;

    /* LSB is at lower address, MSB at offset+1 */
    return (uint16)( (uint16)base[offset]                    |
                    ((uint16)base[offset + 1u] << 8u) );
}

/* -----------------------------------------------------------------------
 * 3. Read 32-bit word (Little-Endian)
 * ----------------------------------------------------------------------- */
uint32 MemScanner_ReadWord(ConstVoidPtr baseAddr, uint32 offset)
{
    const uint8 *base = (const uint8 *)baseAddr;

    return (uint32)( (uint32)base[offset]                     |
                    ((uint32)base[offset + 1u] <<  8u)         |
                    ((uint32)base[offset + 2u] << 16u)         |
                    ((uint32)base[offset + 3u] << 24u) );
}

/* -----------------------------------------------------------------------
 * 4. Write a single byte
 * ----------------------------------------------------------------------- */
void MemScanner_WriteByte(VoidPtr baseAddr, uint32 offset, uint8 value)
{
    uint8 *base = (uint8 *)baseAddr;
    base[offset] = value;
}

/* -----------------------------------------------------------------------
 * 5. Hex dump — prints: [HexDump] 0A 1B 2C ...
 * ----------------------------------------------------------------------- */
void MemScanner_HexDump(ConstVoidPtr baseAddr, uint32 size)
{
    const uint8 *base = (const uint8 *)baseAddr;
    uint32 i;

    printf("[HexDump]");
    for (i = 0u; i < size; i++)
    {
        printf(" %02X", (uint32)base[i]);
    }
    printf("\n");
}

/* -----------------------------------------------------------------------
 * 6. Memory fill
 * ----------------------------------------------------------------------- */
void MemScanner_MemFill(VoidPtr baseAddr, uint32 size, uint8 value)
{
    uint8  *base = (uint8 *)baseAddr;
    uint32  i;

    for (i = 0u; i < size; i++)
    {
        base[i] = value;
    }
}

/* -----------------------------------------------------------------------
 * 7. Memory compare
 *    Returns  0           -> regions are identical
 *    Returns  N (1-indexed) -> offset of first differing byte
 * ----------------------------------------------------------------------- */
sint32 MemScanner_MemCompare(ConstVoidPtr addr1, VoidPtr addr2, uint32 size)
{
    const uint8 *a = (const uint8 *)addr1;
    const uint8 *b = (const uint8 *)addr2;

    uint32 i;
    for (i = 0u; i < size; i++)
    {
        if (a[i] != b[i])
        {
            return (sint32)(i + 1u);    /* 1-indexed */
        }
    }
    return 0;
}

/* -----------------------------------------------------------------------
 * 8. Find first occurrence of a byte
 *    Returns  offset (0-indexed)  -> byte found
 *    Returns  -1                  -> byte not found
 * ----------------------------------------------------------------------- */
sint32 MemScanner_FindByte(ConstVoidPtr baseAddr, uint32 size, uint8 target)
{
    const uint8 *base = (const uint8 *)baseAddr;
    uint32 i;

    for (i = 0u; i < size; i++)
    {
        if (base[i] == target)
        {
            return (sint32)i;           /* 0-indexed */
        }
    }
    return -1;
}

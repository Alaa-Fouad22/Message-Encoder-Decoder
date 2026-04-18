# Project 1 — Embedded Memory & Messaging Library

> Reusable, hardware-agnostic library for safe memory access with a structured message encoder/decoder for a simulated embedded communication link.

## Project Demo Video

[![Watch Demo Video](https://img.shields.io/badge/Watch%20Demo%20Video-Google%20Drive-blue?style=for-the-badge&logo=googledrive)](https://drive.google.com/file/d/151wLWQoR1dUfLHGuGppdlfRyR1rokmwC/view?usp=drive_link)

---

## Team Members

| # | Name | ID |
|---|------|----|
| 1 | Alaa Abd El-Nasser Mohamed | 9230233 |
| 2 | Alaa Ahmed Mobarek | 9230229 |


---

## Project Structure

```
project1/
├── STD_TYPES.h          # Portable fixed-width type aliases
├── BIT_MATH.h           # MISRA-C compliant bit manipulation macros
├── MemScanner.h         # Memory inspection module header
├── MemScanner.c         # Memory inspection module implementation
├── message.h            # Message struct, payload types, prototypes
├── message.c            # Encoder, decoder, handler registration
└── main.c               # Demonstration (C1 + C2)
```

---

## Part A — Foundation Library

### A1: STD_TYPES.h
Defines all portable type aliases used across the project:
- `uint8`, `uint16`, `uint32` — unsigned fixed-width integers
- `sint8`, `sint16`, `sint32` — signed fixed-width integers
- `boolean`, `TRUE`, `FALSE`
- `STD_ReturnType` with `STD_OK` / `STD_FAILURE`
- `VoidPtr` for generic raw memory access
- No raw C types (`int`, `char`, `long`) appear outside this file

### A2: BIT_MATH.h
MISRA-C compliant macro library for bit manipulation:
- `SET_BIT(reg, bit)` — set a single bit
- `CLR_BIT(reg, bit)` — clear a single bit
- `TOG_BIT(reg, bit)` — toggle a single bit
- `GET_BIT(reg, bit)` — read a single bit
- `SET_MASK` / `CLR_MASK` — multi-bit mask operations
- `WRITE_FIELD` / `READ_FIELD` — bit-field region access

All macros use unsigned literals, full parenthesization, and avoid signed-shift operations.

### A3: MemScanner
Hardware-inspired memory inspection module (modeled after OpenOCD / pyOCD):

| Function | Description |
|----------|-------------|
| `MemScanner_ReadByte` | Read 1 byte at a given offset |
| `MemScanner_ReadHalfWord` | Read 16-bit value (Little-Endian) |
| `MemScanner_ReadWord` | Read 32-bit value (Little-Endian) |
| `MemScanner_WriteByte` | Write 1 byte at a given offset |
| `MemScanner_HexDump` | Print memory region as formatted hex |
| `MemScanner_MemFill` | Fill memory region with a byte value |
| `MemScanner_MemCompare` | Compare two memory regions byte-by-byte |
| `MemScanner_FindByte` | Search for first occurrence of a byte |

---

## Part B — Message Encoder / Decoder

### Message Layout (6 bytes, fixed)

```
Byte 0     Byte 1     Bytes 2–5
─────────  ─────────  ─────────────────────
HEADER     LENGTH     PAYLOAD (4 bytes)
```

#### HEADER byte bit-fields:
```
Bits 2:0 → MSG_TYPE   (0–7)
Bit  3   → PRIORITY   (0 = normal, 1 = high)
Bits 7:4 → SEQ_NUM    (0–15, auto-increments)
```

### Message Types

| MSG_TYPE | Name | Payload |
|----------|------|---------|
| 0 | `SENSOR_READING` | lower 16 bits = temperature (×0.1 °C), upper 16 bits = humidity (×0.1 %RH) |
| 1 | `DEVICE_COMMAND` | byte 0 = command ID, byte 1 = parameter, bytes 2–3 = unused (0x00) |

### B1: Message Struct
`message_t` is a `__attribute__((packed))` struct of exactly 6 bytes, verified at compile time with `_Static_assert`.

### B2: Encoder — `messageEncode()`
- Accepts `msgType`, `priority`, and `MessagePayload_t`
- Builds the header byte using bit-fields
- Writes payload bytes into a 6-byte buffer
- Auto-increments `seqNum` (hidden static state, wraps 0–15)
- Returns pointer to internal buffer — **must be copied immediately**

### B3: Decoder — `messageDecode()`
- Accepts a raw `void*` 6-byte buffer
- Parses header, length, and payload into a `message_t` struct
- For `SENSOR_READING`: payload accessible as 32-bit, two 16-bit halves, or 4 individual bytes (via union)
- Automatically calls the registered handler for the decoded message type
- Returns `STD_OK` or `STD_FAILURE`

### B4: Handler Registration — `messageRegisterHandler()`
- Stores up to 8 handler function pointers (one per message type)
- All state is encapsulated inside `message.c` (not visible externally)
- If no handler is registered for a type, decode still succeeds silently

---

## Part C — Demonstration

### C1: Encode / Decode Flow

```
Register handlers (sensor_handler, command_handler)
        │
        ▼
Encode SENSOR_READING
  temp  = 255  (25.5 °C × 10)
  humid = 600  (60.0 %RH × 10)
        │
        ▼
Encode DEVICE_COMMAND
  cmd_id = 0x01, param = 0xFF
        │
        ▼
messageDecode(sensorBuffer)  →  sensor_handler() called automatically
messageDecode(commandBuffer) →  command_handler() called automatically
```

**sensor_handler** prints:
- Raw 32-bit value
- Two 16-bit halves (temp + humidity)
- Four individual bytes

**command_handler** prints:
- Command ID
- Parameter

### C2: MemScanner Demonstration

| Step | Function | What it does |
|------|----------|-------------|
| 6 | `HexDump` | Prints raw bytes of both message buffers |
| 7 | `ReadByte` | Reads HEADER (offset 0) and LENGTH (offset 1) |
| 8 | `ReadHalfWord` | Reads temperature (lower 16 bits) at offset 2 |
| 9 | `ReadWord` | Reads full 32-bit payload at offset 2 |
| 10 | `WriteByte` | Copies sensor buffer, flips PRIORITY bit, dumps both |
| 11 | `MemFill` | Fills empty 6-byte buffer with 0xAA, dumps to verify |
| 12 | `MemCompare` | Compares two identical buffers (→ 0), then modified (→ offset) |
| 13 | `FindByte` | Finds known byte (→ offset), searches missing byte (→ -1) |

---

## Key Design Decisions

- **Union-based payload** — `SensorPayload_t` uses a union so the same 4 bytes are simultaneously accessible as `uint32`, two `uint16` halves, or `uint8 bytes[]` with zero overhead.
- **Static sequence number** — `seqNum` lives as a `static` variable inside `message.c`, invisible to the caller, auto-wrapping at 15.
- **Static encoder buffer** — `messageEncode` returns a pointer to a static internal buffer. Always copy it before the next encode call.
- **Compile-time size check** — `_Static_assert(sizeof(message_t) == 6)` catches accidental padding at build time.
- **Hardware-agnostic types** — All types go through `STD_TYPES.h`; no raw C types appear outside it.

---

## How to Build

```bash
gcc -Wall -Wextra -o project1 main.c message.c MemScanner.c
./project1
```

---

Team name format: `sbe27_embedded_spring26_team##`

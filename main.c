#include <stdio.h>
#include "BIT_MATH.h"
#include "STD_TYPES.h"
#include "MemScanner.h"
#include "message.h"

/* --- 1. Handlers Definitions [cite: 83, 86] --- */

void sensor_handler(const message_t* msg) {
    printf("\n[Handler] SENSOR_READING Detected:\n");
    /* Print as required: 32-bit, two 16-bit, and four individual bytes [cite: 87] */
    printf("  - Raw 32-bit: 0x%08X\n", (uint32)msg->payload.sensor.full_32_Bits);
    printf("  - 16-bit Temp: %u (0.1 C), Humidity: %u (0.1%%)\n",
            msg->payload.sensor.halves.temp, msg->payload.sensor.halves.humidity);
    printf("  - Individual Bytes: %02X %02X %02X %02X\n",
            msg->payload.sensor.bytes[0], msg->payload.sensor.bytes[1],
            msg->payload.sensor.bytes[2], msg->payload.sensor.bytes[3]);
}

void command_handler(const message_t* msg) {
    printf("\n[Handler] DEVICE_COMMAND Detected:\n");
    printf("  - Command ID: 0x%02X\n", msg->payload.command.deviceCommandFields.command_ID);
    printf("  - Parameter: 0x%02X\n", msg->payload.command.deviceCommandFields.parameter);
}

int main() {
    /* Variables for demonstration */
    uint8 sensorBuffer[6];
    uint8 commandBuffer[6];
    message_t decodedMsg;
    MessagePayload_t sensorPayload, commandPayload;

    /* --- C1: Message Encode / Decode [cite: 82] --- */

    /* 1. Register handlers [cite: 83] */
    messageRegisterHandler(0, sensor_handler); /* SENSOR_READING */
    messageRegisterHandler(1, command_handler); /* DEVICE_COMMAND */

    /* 2. Encode SENSOR_READING (25.5C -> 255, 60.0% -> 600) [cite: 84] */
    sensorPayload.sensor.halves.temp = 255u;
    sensorPayload.sensor.halves.humidity = 600u;
    messageEncode(0, 0, sensorPayload, sensorBuffer);

    /* 3. Encode DEVICE_COMMAND (ID 0x01, Param 0xFF) [cite: 85] */
    commandPayload.command.deviceCommandFields.command_ID = 0x01u;
    commandPayload.command.deviceCommandFields.parameter = 0xFFu;
    commandPayload.command.deviceCommandFields.unused = 0x0000u; /* Zero-filled [cite: 59] */
    messageEncode(1, 0, commandPayload, commandBuffer);

    /* 4. Decode messages [cite: 86] */
    messageDecode(sensorBuffer, &decodedMsg);
    messageDecode(commandBuffer, &decodedMsg);

    /* --- C2: Memory Scanner Demonstration [cite: 88-89] --- */
    printf("\n--- MemScanner Demonstration ---\n");

    /* 6. HexDump [cite: 90-91] */
    printf("Sensor Buffer "); MemScanner_HexDump(sensorBuffer, 6u);
    printf("Command Buffer "); MemScanner_HexDump(commandBuffer, 6u);

    /* 7. ReadByte (Header at offset 0, Length at offset 1) [cite: 92] */
    printf("Header Byte: 0x%02X, Length Byte: %u\n",
            MemScanner_ReadByte(sensorBuffer, 0u),
            MemScanner_ReadByte(sensorBuffer, 1u));

    /* 8. ReadHalfWord (Temp value at offset 2) [cite: 93-95] */
    printf("Temperature (ReadHalfWord): %u\n", MemScanner_ReadHalfWord(sensorBuffer, 2u));

    /* 9. ReadWord (Full Payload at offset 2) [cite: 96] */
    printf("Full Payload (ReadWord): 0x%08X\n", (uint32)MemScanner_ReadWord(sensorBuffer, 2u));

    /* 10. WriteByte (Modify Priority in a copy) [cite: 97-98] */
    uint8 sensorCopy[6];
    MemScanner_MemFill(sensorCopy, 6u, 0x00u); /* Clear copy */
    for(uint8 i=0; i<6; i++) sensorCopy[i] = sensorBuffer[i]; /* Manual copy */

    uint8 header = MemScanner_ReadByte(sensorCopy, 0u);
    SET_BIT(header, 3u); /* Set Priority bit (bit 3) [cite: 47, 98] */
    MemScanner_WriteByte(sensorCopy, 0u, header);

    printf("Original "); MemScanner_HexDump(sensorBuffer, 6u);
    printf("Modified "); MemScanner_HexDump(sensorCopy, 6u);

    /* 11. MemFill 0xAA [cite: 100] */
    uint8 emptyBuf[6];
    MemScanner_MemFill(emptyBuf, 6u, 0xAAu);
    printf("Fill 0xAA: "); MemScanner_HexDump(emptyBuf, 6u);

    /* 12. MemCompare [cite: 101-102] */

    // uint8 anotherBuf[6];
    // messageEncode(0, 0, sensorPayload, anotherBuf); /* Encode same message again */
    // printf("Compare identical: %d\n", MemScanner_MemCompare(sensorBuffer, anotherBuf, 6u));
    //
    // MemScanner_WriteByte(anotherBuf, 4u, 0xEEu); /* Modify 5th byte */
    // printf("Compare different: %d (offset)\n", MemScanner_MemCompare(sensorBuffer, anotherBuf, 6u));

    uint8 anotherBuf[6];
    /* Copying manually to ensure identical sequence numbers for comparison */
    for(uint8 i=0; i<6u; i++) {
        anotherBuf[i] = sensorBuffer[i];
    }

    printf("Compare identical: %d\n", (int)MemScanner_MemCompare(sensorBuffer, anotherBuf, 6u));

    /* Modify the 5th byte (Index 4) to test 1-indexed return */
    MemScanner_WriteByte(anotherBuf, 4u, 0xEEu);
    printf("Compare different: %d (offset)\n", (int)MemScanner_MemCompare(sensorBuffer, anotherBuf, 6u));

    /* 13. FindByte [cite: 103-104] */
    printf("Find 0xFF in sensor: %d\n", MemScanner_FindByte(sensorBuffer, 6u, 0xFFu));
    printf("Find 0xCC (non-existent): %d\n", MemScanner_FindByte(sensorBuffer, 6u, 0xCCu));

    return 0;
}
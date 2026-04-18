#ifndef MESSAGE_STRUCT_H
#define MESSAGE_STRUCT_H
#include "STD_TYPES.h"


/* Header */
typedef union {
    uint8 headerFullBits;  /* 8_bit header */
    struct {
        uint8 msgType: 3;
        uint8 priority: 1;
        uint8 seqNum: 4;
    }bits;
}Header_t;

/* SENSOR_READING payload (4 bytes) */
typedef union {
    uint32 full_32_Bits;
    struct {
        uint16 temp;       //lower 16 bit
        uint16 humidity;  //upper 16 bit
    }halves;
    uint8 bytes[4];
}SensorPayload_t;

/* DEVICE_COMMAND payload (4 bytes) */
typedef union {
    uint32 full32DeviceCommand;
    struct {
        uint8 command_ID;   /* Byte 0 */
        uint8 parameter;    /* Byte 1 */
        uint16 unused;      /* Bytes 2-3 (zeros) */
    }deviceCommandFields;
}DeviceCommandPayload_t;

typedef union {
    SensorPayload_t sensor;        // for MSG_TYPE = 0
    DeviceCommandPayload_t command; // for MSG_TYPE = 1
} MessagePayload_t;



//Ensures no padding
typedef struct __attribute__((packed))
{
    Header_t  header;    /*Byte 0 */
    uint8  length;    /*Byte 1 */
    MessagePayload_t payload;   /*Bytes 2-5 */
}message_t;

typedef void (*MessageHandler)(const message_t* msg);

/* function prototypes */
STD_ReturnType messageEncode(uint8 msgType, uint8 priority, MessagePayload_t payload, uint8* buffer);

STD_ReturnType messageDecode(const VoidPtr rawBuffer, message_t* msg );

void messageRegisterHandler(uint8 msgType, MessageHandler handler);
#endif //PROJECT1_MESSAGE_STRUCT_H
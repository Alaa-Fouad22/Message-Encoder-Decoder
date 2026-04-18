//
// Created by Alaa on 4/8/2026.
//
#include "message.h"

static uint8 seqNum = 0;
static MessageHandler handlers[2] = {NULL, NULL};


uint8* messageEncode(uint8 msgType, uint8 priority, MessagePayload_t payload) {

    static uint8 buffer[6];   // fixed 6‑byte message
    /* Build Header */
    Header_t h;
    h.bits.msgType = msgType & 0x07;
    h.bits.priority = priority & 0x01;
    h.bits.seqNum = seqNum & 0x0F;

    buffer[0] = h.headerFullBits;    /* header */
    buffer[1] =4;                    /* length(fixed) */

    /*copy payload (4 bytes) */
    for (int i = 0; i < 4; i++) {
        buffer[2 + i] = payload.sensor.bytes[i];
        // sensor.bytes or command.bytes both map to same 4 bytes
    }

    /* Increment sequence number (wrap 0–15) */
    seqNum = (seqNum + 1) & 0x0F;

    return buffer;
}

STD_ReturnType messageDecode(const VoidPtr rawBuffer, message_t* msg ) {
    if (msg == NULL || rawBuffer == NULL) {
        return STD_FAILURE;
    }
    const uint8* buffer = (const uint8*)rawBuffer;

    /* Extract header */
    msg->header.headerFullBits = buffer[0];

    /* Extract length */
    msg->length = buffer[1];

    /* copy payload */
    for (int i = 0; i < msg->length; i++) {
        /* sensor.bytes or command.bytes both map to same 4 bytes */
        msg->payload.sensor.bytes[i] = buffer[2+i];
    }
    // Call registered handler if available
    uint8 type = msg->header.bits.msgType;
    if (type < 2 && handlers[type] != NULL) {
        handlers[type](msg);
    }
    return STD_SUCCESS;

}


void messageRegisterHandler(uint8 msgType, MessageHandler handler) {
    if (msgType < 2) {
        handlers[msgType] = handler;
    }
}
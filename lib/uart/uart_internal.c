//
// Created by annika on 09.01.20.
//

#include "lib/uart/uart_internal.h"
#include "lib/uart/circularBuffer/circularBuffer.h"
#include "lib/pinDefinition/fpgaRegisters.h"
#include "lib/pinDefinition/fpgaPins.h"
#include "EmbeddedUtilities/BitManipulation.h"
#include "lib/interruptManager/interruptManager.h"

circularBuffer sendingBuf;
void (*uartReceiveHandler)(uint8_t);
uint8_t sendingData;
uint8_t sendingFlag;


uint16_t uart_BufferCount_internal(void) {
    return circularBuffer_Count(&sendingBuf);
}

void uart_setUartReceiveHandler_internal(void (*receiveHandler)(uint8_t)){
    uartReceiveHandler = receiveHandler;
}


void uart_WriteNext_internal(void){
    if(!sendingFlag) {
        /* Wait for empty transmit buffer */
        while(!( BitManipulation_bitIsSetOnArray(UCSR1A, UDRE1))) {}
        circularBuffer_Pop(&sendingBuf, &sendingData);
        UDR1 = sendingData;
        sendingFlag = 0x1;
    }
}

uint8_t uart_Queue_internal(uint8_t c){
    uint8_t retries = 0;
    uint8_t success;
    while (retries < MAX_QUEUE_RETRIES)
    {
        success = circularBuffer_Push(&sendingBuf, c);
        if(!success)
        {
            _delay_ms(1);
            retries++;
        }
        else
            break;
    }
    // queue failed
    if (retries >= MAX_QUEUE_RETRIES)
    {
        uint8_t dummy = 0;
        while(circularBuffer_Pop(&sendingBuf, &dummy))
            continue;
    }
    return success;
}

void uart_WriteCharBlock_internal(uint8_t c) {
    interruptManager_clearInterrupt();
    while(!( BitManipulation_bitIsSetOnArray(UCSR1A, UDRE1))) {}
    // Put data into buffer, sends the data
    UDR1 = c;
    interruptManager_setInterrupt();
}

uint8_t uart_ReceiveCharBlocking_internal(void){
    interruptManager_clearInterrupt();
    // Wait for finished receive
    while(!( BitManipulation_bitIsSetOnArray(UCSR1A, RXC1))) {}
    interruptManager_setInterrupt();
    return UDR1;
}
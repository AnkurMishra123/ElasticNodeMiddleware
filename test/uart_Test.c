//
// Created by annika on 08.01.20.
//

#include "unity.h"
#include "lib/uart/circularBuffer/MockcircularBuffer.h"
#include "lib/uart/uart.h"
#include "lib/uart/Mockuart_internal.h"
#include "lib/interruptManager/MockinterruptManager.h"
#include "lib/pinDefinition/fpgaRegisters.h"
#include "lib/pinDefinition/fpgaPins.h"
#include "test/header_replacements/EmbeddedUtilities/MockBitManipulation.h"

circularBuffer sendingBuf;
void (*uartReceiveHandler)(uint8_t);
uint8_t sendingFlag;


uint8_t ubrr1h;
uint8_t* UBRR1H = &ubrr1h;
uint8_t ubrr1l;
uint8_t* UBRR1L = &ubrr1l;
uint8_t ucsr1a;
uint8_t* UCSR1A = &ucsr1a;
uint8_t ucsr1b;
uint8_t* UCSR1B = &ucsr1b;
uint8_t ucsr1c;
uint8_t* UCSR1C = &ucsr1c;


void initalise_uart_mockRegister(void) {
    UBRR1H = &ubrr1h;
    UBRR1L = &ubrr1l;
    UCSR1A = &ucsr1a;
    UCSR1B = &ucsr1b;
    UCSR1C = &ucsr1c;
}

void test_uart_WaitUntilDone(void) {
    initalise_uart_mockRegister();

    //while-call: expected two times
    uart_BufferCount_internal_ExpectAndReturn(1);
    _delay_ms_Expect(10);
    uart_BufferCount_internal_ExpectAndReturn(0);
    uart_WaitUntilDone();
}

void test_uart_getUartReceiveHandler(void) {
    initalise_uart_mockRegister();

    void* ptr = uart_getUartReceiveHandler();
    TEST_ASSERT_EQUAL(ptr, uartReceiveHandler);
}

void dummyFunction(uint8_t dummy){}

void test_uart_init(void) {
    initalise_uart_mockRegister();

    void (*receiveHandler)(uint8_t) = &dummyFunction;

    //UART_2X is 1
    BitManipulation_setBit_Expect(UCSR1A, U2X1);
    BitManipulation_setBit_Expect(UCSR1B, (RXEN1 | TXEN1 | RXCIE1 | TXCIE1));
    BitManipulation_setBit_Expect(UCSR1C, (USBS1 | ( 3 << UCSZ10)));

    uart_setUartReceiveHandler_internal_Expect(receiveHandler);
    circularBuffer_Init_Expect(&sendingBuf, UART_SENDING_BUFFER);
    interruptManager_setInterrupt_Expect();

    uart_Init(receiveHandler);

    TEST_ASSERT_EQUAL_UINT8(sendingFlag, 0x0);
    TEST_ASSERT_EQUAL(UBRR1H, (uint8_t) (my_bdr >> 8));
    TEST_ASSERT_EQUAL(UBRR1L, (uint8_t) (my_bdr));

}

void test_uart_Sending(void) {
    initalise_uart_mockRegister();

    TEST_ASSERT_EQUAL_UINT8(sendingFlag, uart_Sending());
}

void test_uart_WriteChar(void) {
    initalise_uart_mockRegister();
   uint8_t c = 5;
   //Push successful
   circularBuffer_Push_ExpectAndReturn(&sendingBuf, c, 1);
   uart_WriteNext_internal_Expect();
   uart_WriteChar(c);

   TEST_ASSERT_EQUAL_UINT8(sendingFlag, 0x1);
}

void test_uart_WriteString_EmptyString(void) {
    initalise_uart_mockRegister();
    char *s = "";
    uart_WriteNext_internal_Expect();
    uart_WriteString(s);
}

void test_uart_WriteString(void) {
    initalise_uart_mockRegister();
    char *s = "ai";
    char *dummy = s;
    uart_Queue_internal_ExpectAndReturn(*(dummy++), 1);
    uart_Queue_internal_ExpectAndReturn(*(dummy++), 1);
    uart_WriteNext_internal_Expect();
    uart_WriteString(s);
}

void test_uartWriteStringBlock(void) {
    initalise_uart_mockRegister();
    char *s = "ai";
    char *dummy = s;
    uart_WriteCharBlock_internal_Expect(*(dummy++));
    uart_WriteCharBlock_internal_Expect(*(dummy++));
    uart_WriteStringBlock(s);
}

void test_uart_WriteStringLength(void) {
    initalise_uart_mockRegister();
    char *s = "ai";
    char *dummy = s;
    uint16_t length = 2;
    uart_Queue_internal_ExpectAndReturn(*(dummy++), 1);
    uart_Queue_internal_ExpectAndReturn(*(dummy++), 1);
    uart_WriteNext_internal_Expect();
    uart_WriteStringLength(s, length);
}

void test_uart_WriteStringLengthBlock(void) {
    initalise_uart_mockRegister();
    char *s = "ai";
    char *dummy = s;
    uint16_t length = 2;
    uart_WriteCharBlock_internal_Expect(*(dummy++));
    uart_WriteCharBlock_internal_Expect(*(dummy++));
    uart_WriteStringLengthBlock(s, length);
}

void test_uart_ReceiveUint32Blocking(void) {
    initalise_uart_mockRegister();
    uint32_t output_var;
    uint32_t *output = &output_var;
    /*	cli();
uint8_t *outputPtr = (uint8_t *) output;

*outputPtr++ = uartReceiveCharBlocking();
*outputPtr++ = uartReceiveCharBlocking();
*outputPtr++ = uartReceiveCharBlocking();
*outputPtr++ = uartReceiveCharBlocking();

sei();
// return output;*/
    interruptManager_clearInterrupt_Expect();
    uint8_t *outputPtr = (uint8_t * )output;
    interruptManager_setInterrupt_Expect();
    uart_ReceiveUint32Blocking(output);
}
//NOT TESTET:
//uart_NewLine
//uart_WriteLine
#include "lib/configuration/configuration.h"
#include "lib/debug/debug.h"
#include "lib/elasticNodeMiddleware/elasticNodeMiddleware.h"
#include "lib/flash/flash.h"
#include "lib/interruptManager/interruptManager.h"
#include "lib/led/led_mcu.h"

#define BUFFER_SIZE 256
uint32_t configAddress, configSize, configRemaining;
uint8_t *buffer;

void readData(uint8_t *buffer, uint16_t num);

void readValue(uint32_t *destination)
{
    readData((uint8_t *) destination, sizeof(uint32_t));

    debugWriteStringLength(destination, sizeof(uint32_t));
}


void readData(uint8_t *buffer, uint16_t num)
{
    uint8_t *ptr = buffer;
    for (uint16_t i = 0; i < num; i++) {
        *ptr++ = (uint8_t) debugReadCharBlock();

        /*
         * uncomment to see blinking leds while the progress of uploading the bitfile
         * blinking is very very fast!
        if(i%2==0) {
            led_mcu_turnOn(0);
        } else {
            led_mcu_turnOff(0);
        }
         */

    }

}

void configurationUartFlash(void) {

    elasticnode_fpgaPowerOff();
    elasticnode_fpgaHardReset();
    led_mcu_turnOn(3);

    // getting address
    readValue(&configAddress);
    // getting size
    readValue(&configSize);

    buffer = (uint8_t *) malloc(BUFFER_SIZE);

    flashEnableInterface();

    uint16_t blockSize = BUFFER_SIZE;

    led_mcu_turnOn(2);

    debugWriteString("Erasing flash... ");
    uint16_t numBlocks4K = ceil((float)(configSize) / 0x1000);
    debugWriteDec16(numBlocks4K);
    debugWriteString(" ");
    debugWriteDec32(configSize);
    debugNewLine();
    uint32_t blockAddress;
    for (uint16_t blockCounter = 0; blockCounter < numBlocks4K; blockCounter++)
    {
        blockAddress = configAddress + ((uint32_t) blockCounter) * 0x1000;
        eraseSectorFlash(blockAddress, 1);
    }


    led_mcu_turnOff(2);
    led_mcu_turnOff(3);
    uint32_t currentAddress = configAddress;
    configRemaining = configSize;

    debugReady();
    while(configRemaining > 0) {
        if (configRemaining < BUFFER_SIZE) {
            blockSize = configRemaining;
        }
        led_mcu_turnOn(1);
        readData(buffer, blockSize);
        led_mcu_turnOn(0);

        writeDataFlash(currentAddress, buffer, blockSize, 1);
        debugAck(buffer[blockSize - 1]);

        led_mcu_turnOff(0);
        led_mcu_turnOff(1);

        currentAddress += blockSize;
        configRemaining -= blockSize;


        debugDone();
    }
    free(buffer);

    debugDone();

    interruptManager_setInterrupt();
}

// not used until now
// eventually include verification of flash
void verifyConfigurationFlash(uint8_t mcuFlash)
{
    elasticnode_fpgaHardReset();
    flashEnableInterface();

    led_mcu_turnOn(0);
    readValue(&configAddress);  // getting address
    led_mcu_turnOn(1);
    readValue(&configSize);     // getting size
    led_mcu_turnOff(0);
    led_mcu_turnOff(1);

    led_mcu_turnOn(2);
    buffer = readDataFlash(configAddress, configSize, mcuFlash, NULL, NULL);
    led_mcu_turnOn(3);
    debugWriteStringLength(buffer, configSize);
    led_mcu_turnOff(3);
    led_mcu_turnOff(2);

    debugReady();
    debugDone();


    interruptManager_setInterrupt();
}

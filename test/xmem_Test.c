//
// Created by annika on 01.10.19.
//

#include "unity.h"
#include "elasticnodemiddleware/MockregisterAbstraction.h"
#include "elasticnodemiddleware/xmem.h"
#include "elasticnodemiddleware/fpgaPins.h"

uint8_t xmem_enable_reg;
uint8_t ddr_xmem_a;
uint8_t ddr_xmem_ad;
uint8_t ddr_xmem_wr;
uint8_t ddr_xmem_rd;
uint8_t port_xmem_rd;
uint8_t ddr_xmem_ale;
uint8_t sprc;
uint8_t xmcra;
uint8_t xmcrb;

uint8_t* XMEM_ENABLE_REG = &xmem_enable_reg;
uint8_t* DDR_XMEM_A = &ddr_xmem_a;
uint8_t* DDR_XMEM_AD = &ddr_xmem_ad;
uint8_t* DDR_XMEM_WR = &ddr_xmem_wr;
uint8_t* DDR_XMEM_RD = &ddr_xmem_rd;
uint8_t* PORT_XMEM_RD = &port_xmem_rd;
uint8_t* DDR_XMEM_ALE = &ddr_xmem_ale;
uint8_t* SPCR = &sprc;
uint8_t* XMCRA = &xmcra;
uint8_t* XMCRB = &xmcrb;

void initialise_xmem_mockRegister(void) {
    XMEM_ENABLE_REG = &xmem_enable_reg;
    DDR_XMEM_A = &ddr_xmem_a;
    DDR_XMEM_AD = &ddr_xmem_ad;
    DDR_XMEM_WR = &ddr_xmem_wr;
    PORT_XMEM_RD = &port_xmem_rd;
    DDR_XMEM_ALE = &ddr_xmem_ale;
    DDR_XMEM_RD = &ddr_xmem_rd;
    SPCR = &sprc;
    XMCRA = &xmcra;
    XMCRB = &xmcrb;
}
void test_initXmem(void) {

    initialise_xmem_mockRegister();

    abstraction_setRegisterBitsHigh_Expect(XMCRA, ((1 << SRW11) | (1 << SRW10)));
    abstraction_setRegisterBitsHigh_Expect(XMCRB, ((1 << XMBK) | (1 << XMM1)));

    initXmem();
}

void test_enableXmem(void){

    initialise_xmem_mockRegister();

    //spi disable
    abstraction_setRegisterBitsHigh_Expect(SPCR, (1 << SPE));

    *DDR_XMEM_A = XMEM_A_MASK;
    *DDR_XMEM_ALE = XMEM_AD_MASK;

    abstraction_setRegisterBitsHigh_Expect(DDR_XMEM_WR, (1 << P_XMEM_WR));
    abstraction_setRegisterBitsHigh_Expect(DDR_XMEM_RD, (1 << P_XMEM_RD));
    abstraction_setRegisterBitsHigh_Expect(DDR_XMEM_ALE, (1 << P_XMEM_ALE));

    abstraction_setRegisterBitsHigh_Expect(XMEM_ENABLE_REG, (1 << XMEM_ENABLE_P));

    enableXmem();

}

void test_disableXmem(void) {

    initialise_xmem_mockRegister();

    abstraction_setRegisterBitsLow_Expect(XMEM_ENABLE_REG, (1 << XMEM_ENABLE_P));

    *DDR_XMEM_A = 0x00;
    *DDR_XMEM_ALE = 0x00;

    abstraction_setRegisterBitsLow_Expect(DDR_XMEM_WR, (1 << P_XMEM_WR));
    abstraction_setRegisterBitsHigh_Expect(PORT_XMEM_RD, (1 << P_XMEM_RD));
    abstraction_setRegisterBitsLow_Expect(DDR_XMEM_ALE, (1 << P_XMEM_ALE));

    disableXmem();
}
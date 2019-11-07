//
// Created by annika on 01.10.19.
//

#include "elasticnodemiddleware/xmem.h"
#include "elasticnodemiddleware/registerAbstraction.h"
#include "elasticnodemiddleware/fpgaRegisters.h"
#include "elasticnodemiddleware/fpgaPins.h"


void initXmem(void) {

    /*XMCRA = (1 << SRW11 ) | (1 << SRW10);
    XMCRB = (1 << XMBK) | (1 << XMM1);*/
    //SRW10 in MCUCR

    abstraction_setRegisterBitsHigh(XMCRA, ((1 << SRW11) | (1 << SRW10)));
    abstraction_setRegisterBitsHigh(XMCRB, ((1 << XMBK) | (1 << XMM1)));

}

void enableXmem(void){

    //spi disable
    abstraction_setRegisterBitsHigh(SPCR, (1 << SPE));

    *DDR_XMEM_A = XMEM_A_MASK;
    *DDR_XMEM_ALE = XMEM_AD_MASK;

    abstraction_setRegisterBitsHigh(DDR_XMEM_WR, (1 << P_XMEM_WR));
    abstraction_setRegisterBitsHigh(DDR_XMEM_RD, (1 << P_XMEM_RD));
    abstraction_setRegisterBitsHigh(DDR_XMEM_ALE, (1 << P_XMEM_ALE));

    abstraction_setRegisterBitsHigh(XMEM_ENABLE_REG, (1 << XMEM_ENABLE_P));
}

void disableXmem(void){

    abstraction_setRegisterBitsLow(XMEM_ENABLE_REG, (1<< XMEM_ENABLE_P));

    *DDR_XMEM_A = 0x00;
    *DDR_XMEM_AD = 0x00;

    abstraction_setRegisterBitsLow(DDR_XMEM_WR, (1 <<  P_XMEM_WR));
    abstraction_setRegisterBitsHigh(PORT_XMEM_RD, (1 << P_XMEM_RD));
    abstraction_setRegisterBitsLow(DDR_XMEM_ALE, (1 << P_XMEM_ALE));
}
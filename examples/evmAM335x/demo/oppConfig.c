/**
 *  \file   oppConfig.c
 *
 *  \brief  OPP changing functions are defined in this file
 *
 */

/*
* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
*/
/*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "soc_AM335x.h"
#include "hw_emif4d.h"
#include "hw_types.h"
#include "hw_cm_per.h"
#include "cache.h"
#include "interrupt.h"
#include "hw_cm_wkup.h"
#include "clock.h"

/*    For ARM v7 arch    */
#define     NO_CORE_REGISTERS            40
#define     REG_SIZE                     4
#define     DDR_START_ADDR               0x80000000
#define     PM_DDR_IO_CONTROL            0x0000018B
#define     PM_DELAY_COUNT               15000

#define     OPP50_DDR2_READ_LATENCY      0x05
#define     OPP50_DDR2_SDRAM_TIMING1     0x02224180
#define     OPP50_DDR2_SDRAM_TIMING2     0x241931C2
#define     OPP50_DDR2_SDRAM_TIMING3     0x0000017F
#define     OPP50_DDR2_SDRAM_CONFIG      0x40804F32
#define     OPP50_DDR2_REF_CTRL          0x000003CF

#define     PM_OPP50_DDR_M               125
#define     PM_OPP50_DDR_N               23
#define     PM_OPP50_DDR_M2              1

//DDR2=266MHz
//OPP100
#define     OPP100_DDR2_READ_LATENCY     0x05
#define     OPP100_DDR2_SDRAM_TIMING1    0x0446A391
#define     OPP100_DDR2_SDRAM_TIMING2    0x242431CA
#define     OPP100_DDR2_SDRAM_TIMING3    0x0000021F
#define     OPP100_DDR2_SDRAM_CONFIG     0x40805232
#define     OPP100_DDR2_REF_CTRL         0x0000040D

#define     PM_OPP100_DDR_M              266
#define     PM_OPP100_DDR_N              23
#define     PM_OPP100_DDR_M2             1

#define     CM_PER_EMIF_CLKCTRL_MODULEMODE_DISABLE    CM_PER_EMIF_CLKCTRL_MODULEMODE_DISABLED

#define     DDR_PHY_BASE_ADDR            (0x44E12000)

//CMD0
#define     CMD0_REG_PHY_CTRL_SLAVE_RATIO_0         (0x01C + DDR_PHY_BASE_ADDR)
#define     CMD0_REG_PHY_CTRL_SLAVE_FORCE_0         (0x020 + DDR_PHY_BASE_ADDR)
#define     CMD0_REG_PHY_CTRL_SLAVE_DELAY_0         (0x024 + DDR_PHY_BASE_ADDR)
#define     CMD0_REG_PHY_DLL_LOCK_DIFF_0            (0x028 + DDR_PHY_BASE_ADDR)
#define     CMD0_REG_PHY_INVERT_CLKOUT_0            (0x02C + DDR_PHY_BASE_ADDR)

//CMD1
#define     CMD1_REG_PHY_CTRL_SLAVE_RATIO_0         (0x050 + DDR_PHY_BASE_ADDR)
#define     CMD1_REG_PHY_CTRL_SLAVE_FORCE_0         (0x054 + DDR_PHY_BASE_ADDR)
#define     CMD1_REG_PHY_CTRL_SLAVE_DELAY_0         (0x058 + DDR_PHY_BASE_ADDR)
#define     CMD1_REG_PHY_DLL_LOCK_DIFF_0            (0x05C + DDR_PHY_BASE_ADDR)
#define     CMD1_REG_PHY_INVERT_CLKOUT_0            (0x060 + DDR_PHY_BASE_ADDR)

//CMD2
#define     CMD2_REG_PHY_CTRL_SLAVE_RATIO_0         (0x084 + DDR_PHY_BASE_ADDR)
#define     CMD2_REG_PHY_CTRL_SLAVE_FORCE_0         (0x088 + DDR_PHY_BASE_ADDR)
#define     CMD2_REG_PHY_CTRL_SLAVE_DELAY_0         (0x08C + DDR_PHY_BASE_ADDR)
#define     CMD2_REG_PHY_DLL_LOCK_DIFF_0            (0x090 + DDR_PHY_BASE_ADDR)
#define     CMD2_REG_PHY_INVERT_CLKOUT_0            (0x094 + DDR_PHY_BASE_ADDR)

//DATA0
#define     DATA0_REG_PHY_RD_DQS_SLAVE_RATIO_0      (0x0C8 + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_RD_DQS_SLAVE_RATIO_1      (0x0CC + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_WR_DQS_SLAVE_RATIO_0      (0x0DC + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_WR_DQS_SLAVE_RATIO_1      (0x0E0 + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_WRLVL_INIT_RATIO_0        (0x0F0 + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_WRLVL_INIT_RATIO_1        (0x0F4 + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_GATELVL_INIT_RATIO_0      (0x0FC + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_GATELVL_INIT_RATIO_1      (0x100 + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_FIFO_WE_SLAVE_RATIO_0     (0x108 + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_FIFO_WE_SLAVE_RATIO_1     (0x10C + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_WR_DATA_SLAVE_RATIO_0     (0x120 + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_WR_DATA_SLAVE_RATIO_1     (0x124 + DDR_PHY_BASE_ADDR)
#define     DATA0_REG_PHY_DLL_LOCK_DIFF_0           (0x138 + DDR_PHY_BASE_ADDR)

//DATA1
#define     DATA1_REG_PHY_RD_DQS_SLAVE_RATIO_0      (0x16C + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_RD_DQS_SLAVE_RATIO_1      (0x170 + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_WR_DQS_SLAVE_RATIO_0      (0x180 + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_WR_DQS_SLAVE_RATIO_1      (0x184 + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_WRLVL_INIT_RATIO_0        (0x194 + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_WRLVL_INIT_RATIO_1        (0x198 + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_GATELVL_INIT_RATIO_0      (0x1A0 + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_GATELVL_INIT_RATIO_1      (0x1A4 + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_FIFO_WE_SLAVE_RATIO_0     (0x1AC + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_FIFO_WE_SLAVE_RATIO_1     (0x1B0 + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_WR_DATA_SLAVE_RATIO_0     (0x1C4 + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_WR_DATA_SLAVE_RATIO_1     (0x1C8 + DDR_PHY_BASE_ADDR)
#define     DATA1_REG_PHY_DLL_LOCK_DIFF_0           (0x1DC + DDR_PHY_BASE_ADDR)

//******************************************************************
//DDR2 parameters
//******************************************************************
#define     DDR2_REG_PHY_CTRL_SLAVE_RATIO             0x80
#define     DDR2_PHY_RD_DQS_SLAVE_RATIO               0x12
#define     DDR2_REG_PHY_FIFO_WE_SLAVE_RATIO          0x80
#define     DDR2_REG_PHY_WR_DATA_SLAVE_RATIO          0x40

//*******************************************************************
//DDR IO Control
//*******************************************************************
#define     DDR_IOCTRL_VALUE                          (0x18B)

extern const unsigned int AM335X_VECTOR_BASE;

#ifdef __TMS470__
#pragma CODE_SECTION(ddrPllOpp50Config, "IRAM_CODE");
#pragma CODE_SECTION(ddrPllOpp100Config, "IRAM_CODE");

static volatile unsigned int loopIdx;
#pragma DATA_SECTION(loopIdx, "IRAM_DATA");

#elif defined(__IAR_SYSTEMS_ICC__)
void ddrPllOpp50Config() @ "IRAM_CODE";
void ddrPllOpp100Config() @ "IRAM_CODE";
static volatile unsigned int loopIdx  @ "IRAM_DATA";

#else
void ddrPllOpp50Config() __attribute__ ((section ("IRAM_CODE")));
void ddrPllOpp100Config() __attribute__ ((section ("IRAM_CODE")));
static volatile unsigned int loopIdx  __attribute__ ((section ("IRAM_DATA")));

#endif

//*******************************************************************
//                      FUNCTION DEFINITIONS
//*******************************************************************
void ddrPllOpp50Config(void)
{
    /*    Disable interrupts    */
    IntMasterIRQDisable();
    IntMasterFIQDisable();

    /*    DDR2 in SR    */
    HWREG(SOC_EMIF_0_REGS + EMIF_PWR_MGMT_CTRL) |=
    ((EMIF_PWR_MGMT_CTRL_REG_LP_MODE_SELFREFRESH <<
        EMIF_PWR_MGMT_CTRL_REG_LP_MODE_SHIFT) & EMIF_PWR_MGMT_CTRL_REG_LP_MODE);

    /* Give a delay */
    for(loopIdx = 0;(loopIdx < PM_DELAY_COUNT);loopIdx++) {}

    /*    PLL Configuration    */
    /*    MN bypass    */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) =
        (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) &
            (~CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN)) |
                CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN_DPLL_MN_BYP_MODE;

    while(((HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DDR)) &
        CM_WKUP_CM_IDLEST_DPLL_DDR_ST_MN_BYPASS )!=
            CM_WKUP_CM_IDLEST_DPLL_DDR_ST_MN_BYPASS);

    /*    M & N    */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DDR) =
        (PM_OPP50_DDR_M << CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_MULT_SHIFT) |
            (PM_OPP50_DDR_N);

    /*    M2    */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DDR) =
        (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DDR) &
        (~CM_WKUP_CM_DIV_M2_DPLL_DDR_DPLL_CLKOUT_DIV)) | (PM_OPP50_DDR_M2);

    /*    PLL Relock    */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) =
    (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) &
        (~CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN)) |
            CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN_DPLL_LOCK_MODE;

    while(((HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DDR)) &
        CM_WKUP_CM_IDLEST_DPLL_DDR_ST_DPLL_CLK )!=
            CM_WKUP_CM_IDLEST_DPLL_DDR_ST_DPLL_CLK_DPLL_LOCKED);

    /*    DDR out of SR    */
    HWREG(SOC_EMIF_0_REGS + EMIF_PWR_MGMT_CTRL) &=
    ~((EMIF_PWR_MGMT_CTRL_REG_LP_MODE_SELFREFRESH <<
        EMIF_PWR_MGMT_CTRL_REG_LP_MODE_SHIFT) & EMIF_PWR_MGMT_CTRL_REG_LP_MODE);

    /*    Enable interrupts    */
    IntMasterFIQEnable();
    IntMasterIRQEnable();
}


void ddrPllOpp100Config(void)
{
    /*    Disable interrupts    */
    IntMasterIRQDisable();
    IntMasterFIQDisable();

    /*    DDR2 in SR    */
    HWREG(SOC_EMIF_0_REGS + EMIF_PWR_MGMT_CTRL) |=
    ((EMIF_PWR_MGMT_CTRL_REG_LP_MODE_SELFREFRESH <<
        EMIF_PWR_MGMT_CTRL_REG_LP_MODE_SHIFT) & EMIF_PWR_MGMT_CTRL_REG_LP_MODE);

    /* Give a delay */
    //for(loopIdx = 0;(loopIdx < PM_DELAY_COUNT);loopIdx++) {}

    /*    PLL Configuration    */
    /*    MN bypass    */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) =
        (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) &
            (~CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN)) |
                CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN_DPLL_MN_BYP_MODE;

    while(((HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DDR)) &
        CM_WKUP_CM_IDLEST_DPLL_DDR_ST_MN_BYPASS )!=
            CM_WKUP_CM_IDLEST_DPLL_DDR_ST_MN_BYPASS);

    /*    M & N    */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DDR) =
        (PM_OPP100_DDR_M << CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_MULT_SHIFT) |
            (PM_OPP100_DDR_N);

    /*    M2    */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DDR) =
    (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DDR) &
    (~CM_WKUP_CM_DIV_M2_DPLL_DDR_DPLL_CLKOUT_DIV)) | (PM_OPP100_DDR_M2);

    /*    PLL Relock    */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) =
    (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) &
        (~CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN)) |
            CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN_DPLL_LOCK_MODE;

    while(((HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DDR)) &
        CM_WKUP_CM_IDLEST_DPLL_DDR_ST_DPLL_CLK )!=
            CM_WKUP_CM_IDLEST_DPLL_DDR_ST_DPLL_CLK_DPLL_LOCKED);

    /*    DDR out of SR    */
    HWREG(SOC_EMIF_0_REGS + EMIF_PWR_MGMT_CTRL) &=
    ~((EMIF_PWR_MGMT_CTRL_REG_LP_MODE_SELFREFRESH <<
        EMIF_PWR_MGMT_CTRL_REG_LP_MODE_SHIFT) & EMIF_PWR_MGMT_CTRL_REG_LP_MODE);

    /*    Enable interrupts    */
    IntMasterFIQEnable();
    IntMasterIRQEnable();
}


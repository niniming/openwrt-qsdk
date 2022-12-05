/*
 * Copyright (c) 2014, 2015-2017, 2020 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <pci.h>

#include <linux/sizes.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch-qca-common/gpio.h>
#include <asm/arch-qca-common/iomap.h>
#include <fdtdec.h>
#include <dm.h>

DECLARE_GLOBAL_DATA_PTR;

#define PCIE_RST_CTRL_PIPE_ARES			0x4
#define PCIE_RST_CTRL_PIPE_STICKY_ARES		0x100
#define PCIE_RST_CTRL_PIPE_PHY_AHB_ARES		0x800
#define PCIE_RST_CTRL_AXI_M_ARES		0x1
#define PCIE_RST_CTRL_AXI_M_STICKY_ARES		0x80
#define PCIE_RST_CTRL_AXI_S_ARES		0x2
#define PCIE_RST_CTRL_AHB_ARES			0x400


#define PCI_CFG0_RDWR			0x4
#define PCI_CFG1_RDWR			0x5
#define RD				0
#define WR 				1

#define MSM_PCIE_DEV_CFG_ADDR                   0x01000000
#define PCIE20_PLR_IATU_VIEWPORT		0x900
#define PCIE20_PLR_IATU_CTRL1			0x904
#define PCIE20_PLR_IATU_CTRL2			0x908
#define PCIE20_PLR_IATU_LBAR			0x90C
#define PCIE20_PLR_IATU_UBAR			0x910
#define PCIE20_PLR_IATU_LAR			0x914
#define PCIE20_PLR_IATU_LTAR			0x918
#define PCIE20_PLR_IATU_UTAR			0x91c

/* PCIE20_PARF_PHYS Registers */
#define PARF_SLV_ADDR_SPACE_SIZE		0x16C
#define SLV_ADDR_SPACE_SZ			0x40000000
#define PCIE_0_PCIE20_PARF_LTSSM		0x1B0
#define LTSSM_EN				(1 << 8)
/* PCIE20_PHYS Registers */
#define PCIE_0_PORT_FORCE_REG			0x708
#define PCIE_0_ACK_F_ASPM_CTRL_REG		0x70C
#define L1_ENTRANCE_LATENCY(x)			(x << 27)
#define L0_ENTRANCE_LATENCY(x)			(x << 24)
#define COMMON_CLK_N_FTS(x)			(x << 16)
#define ACK_N_FTS(x)				(x << 8)

#define PCIE_0_GEN2_CTRL_REG			0x80C
#define FAST_TRAINING_SEQ(x)			(x << 0)
#define NUM_OF_LANES(x)				(x << 8)
#define DIRECT_SPEED_CHANGE			(1 << 17)

#define PCIE_0_TYPE0_STATUS_COMMAND_REG_1	0x004
#define PCI_TYPE0_BUS_MASTER_EN			(1 << 2)

#define PCIE_0_MISC_CONTROL_1_REG		0x8BC
#define DBI_RO_WR_EN				(1 << 0)

#define PCIE_0_LINK_CAPABILITIES_REG		0x07C
#define PCIE_CAP_ASPM_OPT_COMPLIANCE		(1 << 22)
#define PCIE_CAP_LINK_BW_NOT_CAP		(1 << 21)
#define PCIE_CAP_DLL_ACTIVE_REP_CAP		(1 << 20)
#define PCIE_CAP_L1_EXIT_LATENCY(x)		(x << 15)
#define PCIE_CAP_L0S_EXIT_LATENCY(x)		(x << 12)
#define PCIE_CAP_MAX_LINK_WIDTH(x)		(x << 4)
#define PCIE_CAP_MAX_LINK_SPEED(x)		(x << 0)

#define PCIE_0_DEVICE_CONTROL2_DEVICE_STATUS2_REG	0x098
#define PCIE_CAP_CPL_TIMEOUT_DISABLE			(1 << 4)
#define PCIE_0_TYPE0_LINK_CONTROL_LINK_STATUS_REG_1	0x080
#define PCIE20_AXI_MSTR_RESP_COMP_CTRL0                 0x818
#define PCIE20_AXI_MSTR_RESP_COMP_CTRL1                 0x81c

#define PCIE20_SIZE   			SZ_4K
#define PCIE_AXI_CONF_SIZE   		SZ_1M

#define PCIE_USB3_PCS_POWER_DOWN_CONTROL		0x804
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN			0x34
#define QSERDES_COM_CLK_ENABLE1				0x38
#define QSERDES_COM_BG_TRIM				0x70
#define QSERDES_COM_LOCK_CMP_EN				0xC8
#define QSERDES_COM_VCO_TUNE_MAP			0x128
#define QSERDES_COM_VCO_TUNE_TIMER1			0x144
#define QSERDES_COM_VCO_TUNE_TIMER2			0x144
#define QSERDES_COM_CMN_CONFIG				0x194
#define PCIE_QSERDES_COM_PLL_IVCO			0x48
#define QSERDES_COM_HSCLK_SEL				0x178
#define PCIE_QSERDES_COM_SVS_MODE_CLK_SEL		0x19C
#define PCIE_QSERDES_COM_CORE_CLK_EN			0x18C
#define QSERDES_COM_CORECLK_DIV				0x184
#define QSERDES_COM_RESETSM_CNTRL			0xB4
#define PCIE_QSERDES_COM_BG_TIMER			0xC
#define PCIE_QSERDES_COM_SYSCLK_EN_SEL			0xAC
#define PCIE_QSERDES_COM_DEC_START_MODE0		0xD0
#define PCIE_QSERDES_COM_DIV_FRAC_START3_MODE0		0xE4
#define PCIE_QSERDES_COM_DIV_FRAC_START2_MODE0		0xE0
#define PCIE_QSERDES_COM_DIV_FRAC_START1_MODE0		0xDC
#define QSERDES_COM_LOCK_CMP3_MODE0			0x54
#define QSERDES_COM_LOCK_CMP2_MODE0			0x50
#define PCIE_QSERDES_COM_LOCK_CMP1_MODE0		0x4C
#define QSERDES_COM_CLK_SELECT				0x174
#define PCIE_QSERDES_COM_SYS_CLK_CTRL			0x3C
#define QSERDES_COM_SYSCLK_BUF_ENABLE			0x40
#define QSERDES_COM_CP_CTRL_MODE0			0x78
#define QSERDES_COM_PLL_RCTRL_MODE0			0x84
#define QSERDES_COM_PLL_CCTRL_MODE0			0x90
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE0		0x10C
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE0		0x108
#define QSERDES_COM_BIAS_EN_CTRL_BY_PSM			0xA8
#define QSERDES_COM_VCO_TUNE_CTRL			0xC
#define QSERDES_COM_SSC_EN_CENTER			0x10
#define PCIE_QSERDES_COM_SSC_PER1			0x1C
#define QSERDES_COM_SSC_PER2				0x20
#define QSERDES_COM_SSC_ADJ_PER1			0x14
#define QSERDES_COM_SSC_ADJ_PER2			0x18
#define QSERDES_COM_SSC_STEP_SIZE1			0x24
#define QSERDES_COM_SSC_STEP_SIZE2			0x28
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN     0x268
#define QSERDES_TX_LANE_MODE                            0x294
#define QSERDES_TX_RES_CODE_LANE_OFFSET                 0x254
#define PCIE_QSERDES_TX_RCV_DETECT_LVL_2                0x2AC
#define QSERDES_RX_SIGDET_ENABLES                       0x510
#define PCIE_QSERDES_RX_SIGDET_DEGLITCH_CNTRL           0x51C
#define PCIE_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2           0x4D8
#define PCIE_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3           0x4DC
#define PCIE_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4           0x4E0
#define QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE        0x448
#define PCIE_QSERDES_RX_UCDR_SO_GAIN                    0x41C
#define QSERDES_RX_UCDR_SO_GAIN_HALF                    0x410
#define QSERDES_COM_CLK_EP_DIV                          0x74
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE             0x854
#define PCIE_USB3_PCS_OSC_DTCT_ACTIONS                  0x9AC
#define PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK       0x8A0
#define PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_MSB   0x9E0
#define PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_LSB   0x9DC
#define PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME_AUXCLK_LSB  0x9A8
#define PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK         0x8A4
#define PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME             0x8A8
#define QSERDES_RX_SIGDET_CNTRL                         0x514
#define PCIE_USB3_PCS_RX_SIGDET_LVL                     0x9D8
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V0                  0x824
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V0                0x828
#define PCIE_USB3_PCS_SW_RESET                          0x800
#define PCIE_USB3_PCS_START_CONTROL                     0x808

#define PCIE20_PARF_PHY_CTRL		0x40
#define PHY_CTRL_PHY_TX0_TERM_OFFSET_MASK       (0x1f << 16)
#define PHY_CTRL_PHY_TX0_TERM_OFFSET(x)         (x << 16)

#define PCIE20_PARF_PHY_REFCLK		0x4C
#define REF_SSP_EN				BIT(16)
#define REF_USE_PAD				BIT(12)

#define PCIE20_PARF_PCS_DEEMPH		0x34
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN1(x) (x << 16)
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_3_5DB(x) (x << 8)
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_6DB(x) (x << 0)

#define PCIE20_PARF_PCS_SWING		0x38
#define PCIE20_PARF_PCS_SWING_TX_SWING_FULL(x)	(x << 8)
#define PCIE20_PARF_PCS_SWING_TX_SWING_LOW(x)	(x << 0)

#define PCIE20_PARF_CONFIG_BITS		0x50

#define PCIE_SFAB_AXI_S5_FCLK_CTL 	0x00902154

#define PCIE20_ELBI_SYS_CTRL		0x04

#define PCS_COM_POWER_DOWN_CONTROL		0x840
#define PCIE_0_QSERDES_PLL_BIAS_EN_CLKBUFLR_EN	0x03C
#define PCIE_0_QSERDES_PLL_BIAS_EN_CTRL_BY_PSM	0x0A4
#define PCIE_0_QSERDES_PLL_CLK_SELECT		0x16C
#define PCIE_0_QSERDES_PLL_PLL_IVCO		0x050

#define PCIE_0_QSERDES_PLL_BG_TRIM		0x074
#define PCIE_0_QSERDES_PLL_CMN_CONFIG		0x18C
#define PCIE_0_QSERDES_PLL_LOCK_CMP_EN		0x0C4
#define PCIE_0_QSERDES_PLL_RESETSM_CNTRL	0x0B0
#define PCIE_0_QSERDES_PLL_SVS_MODE_CLK_SEL	0x194
#define PCIE_0_QSERDES_PLL_VCO_TUNE_MAP		0x120
#define PCIE_0_QSERDES_PLL_VCO_TUNE_TIMER1	0x13C
#define PCIE_0_QSERDES_PLL_VCO_TUNE_TIMER2	0x140
#define PCIE_0_QSERDES_PLL_CORE_CLK_EN		0x184
#define PCIE_0_QSERDES_PLL_HSCLK_SEL		0x170
#define PCIE_0_QSERDES_PLL_DEC_START_MODE0	0x0CC
#define PCIE_0_QSERDES_PLL_DIV_FRAC_START3_MODE0 0x0E0
#define PCIE_0_QSERDES_PLL_DIV_FRAC_START2_MODE0 0x0DC
#define PCIE_0_QSERDES_PLL_DIV_FRAC_START1_MODE0 0x0D8
#define PCIE_0_QSERDES_PLL_LOCK_CMP2_MODE0	0x058
#define PCIE_0_QSERDES_PLL_LOCK_CMP1_MODE0	0x054
#define PCIE_0_QSERDES_PLL_CP_CTRL_MODE0	0x080
#define PCIE_0_QSERDES_PLL_PLL_RCTRL_MODE0	0x088
#define PCIE_0_QSERDES_PLL_PLL_CCTRL_MODE0	0x090
#define PCIE_0_QSERDES_PLL_INTEGLOOP_GAIN1_MODE0 0x104
#define PCIE_0_QSERDES_PLL_INTEGLOOP_GAIN0_MODE0 0x100
#define PCIE_0_QSERDES_PLL_VCO_TUNE2_MODE0	0x128
#define PCIE_0_QSERDES_PLL_VCO_TUNE1_MODE0	0x124
#define PCIE_0_QSERDES_PLL_CORECLK_DIV		0x17C
#define PCIE_0_QSERDES_PLL_SYS_CLK_CTRL		0x044
#define PCIE_0_QSERDES_PLL_SYSCLK_BUF_ENABLE	0x048
#define PCIE_0_QSERDES_PLL_SYSCLK_EN_SEL	0x0A8
#define PCIE_0_QSERDES_PLL_BG_TIMER		0x00C
#define PCIE_0_QSERDES_PLL_DEC_START_MODE1	0x0D0
#define PCIE_0_QSERDES_PLL_DIV_FRAC_START3_MODE1 0x0EC
#define PCIE_0_QSERDES_PLL_DIV_FRAC_START2_MODE1 0x0E8
#define PCIE_0_QSERDES_PLL_DIV_FRAC_START1_MODE1 0x0E4
#define PCIE_0_QSERDES_PLL_LOCK_CMP2_MODE1	0x064
#define PCIE_0_QSERDES_PLL_LOCK_CMP1_MODE1	0x060
#define PCIE_0_QSERDES_PLL_CP_CTRL_MODE1	0x084
#define PCIE_0_QSERDES_PLL_PLL_RCTRL_MODE1	0x08C
#define PCIE_0_QSERDES_PLL_PLL_CCTRL_MODE1	0x094
#define PCIE_0_QSERDES_PLL_INTEGLOOP_GAIN1_MODE1 0x10C
#define PCIE_0_QSERDES_PLL_INTEGLOOP_GAIN0_MODE1 0x108
#define PCIE_0_QSERDES_PLL_VCO_TUNE2_MODE1	0x130
#define PCIE_0_QSERDES_PLL_VCO_TUNE1_MODE1	0x12C
#define PCIE_0_QSERDES_PLL_CORECLK_DIV_MODE1	0x1B4
#define PCIE_0_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX 0x23C
#define PCIE_0_QSERDES_TX0_RCV_DETECT_LVL_2	0x29C
#define PCIE_0_QSERDES_TX0_HIGHZ_DRVR_EN	0x258
#define PCIE_0_QSERDES_TX0_LANE_MODE_1		0x284
#define PCIE_0_QSERDES_RX0_SIGDET_CNTRL		0x51C
#define	PCIE_0_QSERDES_RX0_SIGDET_ENABLES	0x518
#define PCIE_0_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL 0x524
#define PCIE_0_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2 0x4EC
#define PCIE_0_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3 0x4F0
#define PCIE_0_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4 0x4F4
#define PCIE_0_QSERDES_RX0_DFE_EN_TIMER		0x5B4
#define PCIE_0_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE 0x434
#define PCIE_0_QSERDES_RX0_UCDR_PI_CONTROLS	0x444
#define PCIE_0_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1 0x510
#define PCIE_0_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2 0x514
#define PCIE_0_QSERDES_RX0_RX_MODE_10_LOW	0x598
#define PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH	0x59C
#define PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH2	0x5A0
#define PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH3	0x5A4
#define PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH4	0x5A8
#define PCIE_0_QSERDES_RX0_RX_MODE_01_LOW	0x584
#define PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH	0x588
#define PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH2	0x58C
#define PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH3	0x590
#define PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH4	0x594
#define PCIE_0_QSERDES_RX0_RX_MODE_00_LOW	0x570
#define PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH	0x574
#define PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH2	0x578
#define PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH3	0x57C
#define PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH4	0x580
#define PCIE_0_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH 0x4FC
#define PCIE_0_QSERDES_RX0_RX_IDAC_TSETTLE_LOW	0x4F8

#define PCIE_0_PCS_COM_FLL_CNTRL2		0x89C
#define PCIE_0_PCS_COM_FLL_CNT_VAL_L		0x8A0
#define PCIE_0_PCS_COM_FLL_CNT_VAL_H_TOL	0x8A4
#define PCIE_0_PCS_COM_FLL_MAN_CODE		0x8A8
#define PCIE_0_PCS_COM_FLL_CNTRL1		0x898

#define PCIE_0_PCS_COM_P2U3_WAKEUP_DLY_TIME_AUXCLK_H 0x9A8
#define PCIE_0_PCS_COM_P2U3_WAKEUP_DLY_TIME_AUXCLK_L 0x9A4

#define PCIE_0_PCS_PCIE_OSC_DTCT_ACTIONS	0xC90
#define PCIE_0_PCS_PCIE_L1P1_WAKEUP_DLY_TIME_AUXCLK_H 0xC44
#define PCIE_0_PCS_PCIE_L1P1_WAKEUP_DLY_TIME_AUXCLK_L 0xC40
#define PCIE_0_PCS_PCIE_L1P2_WAKEUP_DLY_TIME_AUXCLK_H 0xC4C
#define PCIE_0_PCS_PCIE_L1P2_WAKEUP_DLY_TIME_AUXCLK_L 0xC48
#define PCIE_0_PCS_PCIE_EQ_CONFIG1		0xCA0
#define PCIE_0_PCS_PCIE_EQ_CONFIG2		0xCA4

#define PCIE_0_PCS_PCIE_POWER_STATE_CONFIG4	0xC14
#define PCIE_0_PCS_PCIE_OSC_DTCT_CONFIG2	0xC5C
#define PCIE_0_PCS_PCIE_OSC_DTCT_MODE2_CONFIG2	0xC78
#define PCIE_0_PCS_PCIE_OSC_DTCT_MODE2_CONFIG4	0xC80
#define PCIE_0_PCS_PCIE_OSC_DTCT_MODE2_CONFIG5	0xC84

#define PCIE_0_QSERDES_PLL_CLK_EP_DIV_MODE0	0x078
#define PCIE_0_QSERDES_PLL_CLK_EP_DIV_MODE1	0x07C
#define PCIE_0_QSERDES_PLL_CLK_ENABLE1		0x040

#define PCIE_0_PCS_COM_POWER_DOWN_CONTROL	0x840
#define PCIE_0_PCS_PCIE_ENDPOINT_REFCLK_DRIVE	0xC1C
#define PCIE_0_PCS_COM_RX_DCC_CAL_CONFIG	0x9D8
#define PCIE_0_PCS_COM_RX_SIGDET_LVL		0x988
#define PCIE_0_PCS_COM_REFGEN_REQ_CONFIG1	0x8DC
#define PCIE_0_PCS_COM_SW_RESET			0x800
#define PCIE_0_PCS_COM_START_CONTROL		0x844

#define PCIE_0_QSERDES_PLL_SSC_PER1		0x01C
#define PCIE_0_QSERDES_PLL_SSC_PER2		0x020
#define PCIE_0_QSERDES_PLL_SSC_STEP_SIZE1_MODE0	0x024
#define PCIE_0_QSERDES_PLL_SSC_STEP_SIZE2_MODE0	0x028
#define PCIE_0_QSERDES_PLL_SSC_STEP_SIZE1_MODE1	0x02C
#define PCIE_0_QSERDES_PLL_SSC_STEP_SIZE2_MODE1	0x030
#define PCIE_0_QSERDES_RX0_UCDR_FO_GAIN		0x408
#define PCIE_0_QSERDES_RX0_UCDR_SO_GAIN		0x414
#define PCIE_0_PCS_COM_G12S1_TXDEEMPH_M3P5DB	0x96C
#define PCIE_0_PCS_COM_EQ_CONFIG5		0x9EC
#define PCIE_0_PCS_PCIE_POWER_STATE_CONFIG2	0xC0C
#define PCIE_0_PCS_PCIE_PRESET_P10_PRE		0xCBC
#define PCIE_0_PCS_PCIE_PRESET_P10_POST		0xCE0

#define PARF_MHI_CLOCK_RESET_CTRL		0x174
#define BYPASS					BIT(4)
#define MSTR_AXI_CLK_EN				BIT(1)
#define AHB_CLK_EN				BIT(0)

#define PCIE_PARF_DEVICE_TYPE			0x1000
#define DEVICE_TYPE_RC				0x4


#define PCIE30_GEN3_RELATED_OFF			0x890
#define GEN3_EQUALIZATION_DISABLE		BIT(16)
#define RXEQ_RGRDLESS_RXTS			BIT(13)
#define GEN3_ZRXDC_NONCOMPL			BIT(0)

#define PCIE20_PARF_SYS_CTRL			0x00
#define ECAM_BLOCKER_EN_RANGE2			BIT(30)
#define MAC_PHY_POWERDOWN_IN_P2_D_MUX_EN	BIT(29)
#define ECAM_REMOVE_OFFSET_EN			BIT(27)
#define ECAM_BLOCKER_EN				BIT(26)
#define MST_WAKEUP_EN				BIT(13)
#define SLV_WAKEUP_EN				BIT(12)
#define MSTR_ACLK_CGC_DIS			BIT(10)
#define SLV_ACLK_CGC_DIS			BIT(9)
#define CORE_CLK_CGC_DIS			BIT(6)
#define AUX_PWR_DET				BIT(4)
#define CORE_CLK_2AUX_CLK_MUX_DIS		BIT(3)
#define L23_CLK_RMV_DIS				BIT(2)
#define L1_CLK_RMV_DIS				BIT(1)

#define PARF_BLOCK_SLV_AXI_WR_BASE		0x360
#define PARF_BLOCK_SLV_AXI_WR_LIMIT		0x368
#define PARF_BLOCK_SLV_AXI_RD_BASE		0x370
#define PARF_BLOCK_SLV_AXI_RD_LIMIT 		0x378
#define PARF_ECAM_BASE				0x380
#define PARF_ECAM_OFFSET_REMOVAL_BASE		0x388
#define PARF_ECAM_OFFSET_REMOVAL_LIMIT		0x390
#define PARF_BLOCK_SLV_AXI_WR_BASE_2		0x398
#define PARF_BLOCK_SLV_AXI_WR_LIMIT_2		0x3A0
#define PARF_BLOCK_SLV_AXI_RD_BASE_2		0x3A8
#define PARF_BLOCK_SLV_AXI_RD_LIMIT_2		0x3B0

#define PCIE20_LNK_CONTROL2_LINK_STATUS2        0xA0
#define PCIE_CAP_CURR_DEEMPHASIS		BIT(16)
#define SPEED_GEN3				0x3

#define PCIE_PHY_DELAY_MS			0xFFFFFFFF

static unsigned int local_buses[] = { 0, 0 };
struct pci_controller pci_hose[PCI_MAX_DEVICES];
static int phy_initialised;
extern int get_soc_version(uint32_t *soc_ver_major, uint32_t *soc_ver_minor);

struct phy_regs {
	u32 reg_offset;
	u32 val;
};

static const struct phy_regs pcie_phy_v2_init_seq_ipq[] = {
#if !defined(CONFIG_IPQ6018)
	{ PCS_COM_POWER_DOWN_CONTROL,				0x00000001},
	{ PCIE_0_QSERDES_PLL_BIAS_EN_CLKBUFLR_EN,		0x00000018},
	{ PCIE_0_QSERDES_PLL_BIAS_EN_CTRL_BY_PSM,		0x00000001},
	{ PCIE_0_QSERDES_PLL_CLK_SELECT,			0x00000031},
	{ PCIE_0_QSERDES_PLL_PLL_IVCO,				0x0000000F},
	{ PCIE_0_QSERDES_PLL_BG_TRIM,				0x0000000F},
	/* delay 5ms */
	{ PCIE_PHY_DELAY_MS,					0x00000005},
	{ PCIE_0_QSERDES_PLL_CMN_CONFIG,			0x00000006},
	{ PCIE_0_QSERDES_PLL_LOCK_CMP_EN,			0x00000042},
	{ PCIE_0_QSERDES_PLL_RESETSM_CNTRL,			0x00000020},
	{ PCIE_0_QSERDES_PLL_SVS_MODE_CLK_SEL,			0x00000001},
	{ PCIE_0_QSERDES_PLL_VCO_TUNE_MAP,			0x00000004},
	{ PCIE_0_QSERDES_PLL_SVS_MODE_CLK_SEL,			0x00000005},
	{ PCIE_0_QSERDES_PLL_VCO_TUNE_TIMER1,			0x000000FF},
	{ PCIE_0_QSERDES_PLL_VCO_TUNE_TIMER2,			0x0000003F},
	{ PCIE_0_QSERDES_PLL_CORE_CLK_EN,			0x00000030},
	{ PCIE_0_QSERDES_PLL_HSCLK_SEL,				0x00000021},
	{ PCIE_0_QSERDES_PLL_DEC_START_MODE0,			0x00000082},
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START3_MODE0,		0x00000003},
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START2_MODE0,		0x00000355},
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START1_MODE0,		0x00035555},
	{ PCIE_0_QSERDES_PLL_LOCK_CMP2_MODE0,			0x0000001A},
	{ PCIE_0_QSERDES_PLL_LOCK_CMP1_MODE0,			0x00001A0A},
	{ PCIE_0_QSERDES_PLL_CP_CTRL_MODE0,			0x0000000B},
	{ PCIE_0_QSERDES_PLL_PLL_RCTRL_MODE0,			0x00000016},
	{ PCIE_0_QSERDES_PLL_PLL_CCTRL_MODE0,			0x00000028},
	{ PCIE_0_QSERDES_PLL_INTEGLOOP_GAIN1_MODE0,		0x00000000},
	{ PCIE_0_QSERDES_PLL_INTEGLOOP_GAIN0_MODE0,		0x00000040},
	{ PCIE_0_QSERDES_PLL_VCO_TUNE2_MODE0,			0x00000002},
	{ PCIE_0_QSERDES_PLL_VCO_TUNE1_MODE0,			0x00000024},
	{ PCIE_0_QSERDES_PLL_SVS_MODE_CLK_SEL,			0x00000005},
	{ PCIE_0_QSERDES_PLL_CORE_CLK_EN,			0x00000020},
	{ PCIE_0_QSERDES_PLL_CORECLK_DIV,			0x0000000A},
	{ PCIE_0_QSERDES_PLL_CLK_SELECT,			0x00000032},
	{ PCIE_0_QSERDES_PLL_SYS_CLK_CTRL,			0x00000002},
	{ PCIE_0_QSERDES_PLL_SYSCLK_BUF_ENABLE,			0x00000007},
	{ PCIE_0_QSERDES_PLL_SYSCLK_EN_SEL,			0x00000008},
	{ PCIE_0_QSERDES_PLL_BG_TIMER,				0x0000000A},
	{ PCIE_0_QSERDES_PLL_HSCLK_SEL,				0x00000001},
	{ PCIE_0_QSERDES_PLL_DEC_START_MODE1,			0x00000068},
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START3_MODE1,		0x00000002},
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START2_MODE1,		0x000002AA},
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START1_MODE1,		0x0002AAAB},
	{ PCIE_0_QSERDES_PLL_LOCK_CMP2_MODE1,			0x00000034},
	{ PCIE_0_QSERDES_PLL_LOCK_CMP1_MODE1,			0x00003414},
	{ PCIE_0_QSERDES_PLL_CP_CTRL_MODE1,			0x0000000B},
	{ PCIE_0_QSERDES_PLL_PLL_RCTRL_MODE1,			0x00000016},
	{ PCIE_0_QSERDES_PLL_PLL_CCTRL_MODE1,			0x00000028},
	{ PCIE_0_QSERDES_PLL_INTEGLOOP_GAIN1_MODE1,		0x00000000},
	{ PCIE_0_QSERDES_PLL_INTEGLOOP_GAIN0_MODE1,		0x00000040},
	{ PCIE_0_QSERDES_PLL_VCO_TUNE2_MODE1,			0x00000003},
	{ PCIE_0_QSERDES_PLL_VCO_TUNE1_MODE1,			0x000000B4},
	{ PCIE_0_QSERDES_PLL_SVS_MODE_CLK_SEL,			0x00000005},
	{ PCIE_0_QSERDES_PLL_CORE_CLK_EN,			0x00000000},
	{ PCIE_0_QSERDES_PLL_CORECLK_DIV_MODE1,			0x00000008},
	/*qmp_tx_init*/
	{ PCIE_0_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX,		0x00000002},
	{ PCIE_0_QSERDES_TX0_RCV_DETECT_LVL_2,			0x00000012},
	{ PCIE_0_QSERDES_TX0_HIGHZ_DRVR_EN,			0x00000010},
	{ PCIE_0_QSERDES_TX0_LANE_MODE_1,			0x00000006},
	/*qmp_rx_init*/
	{ PCIE_0_QSERDES_RX0_SIGDET_CNTRL,			0x00000003},
	{ PCIE_0_QSERDES_RX0_SIGDET_ENABLES,			0x0000001C},
	{ PCIE_0_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL,		0x00000014},
	{ PCIE_0_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2,		0x0000000E},
	{ PCIE_0_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3,		0x00000004},
	{ PCIE_0_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4,		0x0000001B},
	{ PCIE_0_QSERDES_RX0_DFE_EN_TIMER,			0x00000004},
	{ PCIE_0_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE,	0x0000007F},
	{ PCIE_0_QSERDES_RX0_UCDR_PI_CONTROLS,			0x00000070},
	{ PCIE_0_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1,	0x00000073},
	{ PCIE_0_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2,		0x00000080},
	{ PCIE_0_QSERDES_RX0_RX_MODE_10_LOW,			0x00000000},
	{ PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH,			0x00000002},
	{ PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH2,			0x000000C8},
	{ PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH3,			0x00000009},
	{ PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH4,			0x000000B1},
	{ PCIE_0_QSERDES_RX0_RX_MODE_01_LOW,			0x00000000},
	{ PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH,			0x00000002},
	{ PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH2,			0x000000C8},
	{ PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH3,			0x00000009},
	{ PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH4,			0x000000B1},
	{ PCIE_0_QSERDES_RX0_RX_MODE_00_LOW,			0x000000F0},
	{ PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH,			0x00000002},
	{ PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH2,			0x0000002F},
	{ PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH3,			0x000000D3},
	{ PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH4,			0x00000040},
	{ PCIE_0_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH,		0x00000000},
	{ PCIE_0_QSERDES_RX0_RX_IDAC_TSETTLE_LOW,		0x000000C0},
	/* pcie fll config*/
	{ PCIE_0_PCS_COM_FLL_CNTRL2,				0x00000083},
	{ PCIE_0_PCS_COM_FLL_CNT_VAL_L,				0x00000009},
	{ PCIE_0_PCS_COM_FLL_CNT_VAL_H_TOL,			0x000000A2},
	{ PCIE_0_PCS_COM_FLL_MAN_CODE,				0x00000040},
	{ PCIE_0_PCS_COM_FLL_CNTRL1,				0x00000001},
	{ PCIE_0_PCS_COM_P2U3_WAKEUP_DLY_TIME_AUXCLK_H, 	0x00000000},
	{ PCIE_0_PCS_COM_P2U3_WAKEUP_DLY_TIME_AUXCLK_L, 	0x00000001},
	{ PCIE_0_PCS_PCIE_OSC_DTCT_ACTIONS,			0x00000000},
	{ PCIE_0_PCS_PCIE_L1P1_WAKEUP_DLY_TIME_AUXCLK_H,	0x00000000},
	{ PCIE_0_PCS_PCIE_L1P1_WAKEUP_DLY_TIME_AUXCLK_L,	0x00000001},
	{ PCIE_0_PCS_PCIE_L1P2_WAKEUP_DLY_TIME_AUXCLK_H,	0x00000000},
	{ PCIE_0_PCS_PCIE_L1P2_WAKEUP_DLY_TIME_AUXCLK_L,	0x00000001},
	{ PCIE_0_PCS_PCIE_EQ_CONFIG1,				0x00000011},
	{ PCIE_0_PCS_PCIE_EQ_CONFIG2,				0x0000000B},
	{ PCIE_0_PCS_PCIE_POWER_STATE_CONFIG4,			0x00000007},
	{ PCIE_0_PCS_PCIE_OSC_DTCT_CONFIG2,			0x00000052},
	{ PCIE_0_PCS_PCIE_OSC_DTCT_MODE2_CONFIG2,		0x00000050},
	{ PCIE_0_PCS_PCIE_OSC_DTCT_MODE2_CONFIG4,		0x0000001A},
	{ PCIE_0_PCS_PCIE_OSC_DTCT_MODE2_CONFIG5,		0x00000006},
	{ PCIE_0_QSERDES_PLL_CLK_EP_DIV_MODE0,			0x00000019},
	{ PCIE_0_QSERDES_PLL_CLK_EP_DIV_MODE1,			0x00000028},
	{ PCIE_0_QSERDES_PLL_CLK_ENABLE1,			0x00000090},
	{ PCIE_0_PCS_COM_POWER_DOWN_CONTROL,			0x00000003},
	{ PCIE_0_PCS_PCIE_ENDPOINT_REFCLK_DRIVE,		0x000000C1},
	{ PCIE_0_PCS_COM_RX_DCC_CAL_CONFIG,			0x00000001},
	{ PCIE_0_PCS_COM_RX_SIGDET_LVL,				0x000000AA},
	{ PCIE_0_PCS_COM_REFGEN_REQ_CONFIG1,			0x0000000D},
	{ PCIE_0_PCS_COM_SW_RESET,				0x00000000},
	{ PCIE_0_PCS_COM_START_CONTROL,				0x00000003},
#else
	{ PCIE_0_PCS_COM_POWER_DOWN_CONTROL,			0x03 },
	{ PCIE_0_QSERDES_PLL_SSC_PER1,				0x7D },
	{ PCIE_0_QSERDES_PLL_SSC_PER2,				0x01 },
	{ PCIE_0_QSERDES_PLL_SSC_STEP_SIZE1_MODE0,		0x0A },
	{ PCIE_0_QSERDES_PLL_SSC_STEP_SIZE2_MODE0,		0x05 },
	{ PCIE_0_QSERDES_PLL_SSC_STEP_SIZE1_MODE1,		0x08 },
	{ PCIE_0_QSERDES_PLL_SSC_STEP_SIZE2_MODE1,		0x04 },
	{ PCIE_0_QSERDES_PLL_BIAS_EN_CLKBUFLR_EN,		0x18 },
	{ PCIE_0_QSERDES_PLL_CLK_ENABLE1,			0x90 },
	{ PCIE_0_QSERDES_PLL_SYS_CLK_CTRL,			0x02 },
	{ PCIE_0_QSERDES_PLL_SYSCLK_BUF_ENABLE,			0x07 },
	{ PCIE_0_QSERDES_PLL_PLL_IVCO,				0x0F },
	{ PCIE_0_QSERDES_PLL_LOCK_CMP1_MODE0,			0xD4 },
	{ PCIE_0_QSERDES_PLL_LOCK_CMP2_MODE0,			0x14 },
	{ PCIE_0_QSERDES_PLL_LOCK_CMP1_MODE1,			0xAA },
	{ PCIE_0_QSERDES_PLL_LOCK_CMP2_MODE1,			0x29 },
	{ PCIE_0_QSERDES_PLL_BG_TRIM,				0x0F },
	{ PCIE_0_QSERDES_PLL_CP_CTRL_MODE0,			0x09 },
	/*PCIE_0_ delay 5ms */
	{ PCIE_PHY_DELAY_MS,					0x05 },
	{ PCIE_0_QSERDES_PLL_CP_CTRL_MODE1,			0x09 },
	{ PCIE_0_QSERDES_PLL_PLL_RCTRL_MODE0,			0x16 },
	{ PCIE_0_QSERDES_PLL_PLL_RCTRL_MODE1,			0x16 },
	{ PCIE_0_QSERDES_PLL_PLL_CCTRL_MODE0,			0x28 },
	{ PCIE_0_QSERDES_PLL_PLL_CCTRL_MODE1,			0x28 },
	{ PCIE_0_QSERDES_PLL_BIAS_EN_CTRL_BY_PSM,		0x01 },
	{ PCIE_0_QSERDES_PLL_SYSCLK_EN_SEL,			0x08 },
	{ PCIE_0_QSERDES_PLL_RESETSM_CNTRL,			0x20 },
	{ PCIE_0_QSERDES_PLL_LOCK_CMP_EN,			0x42 },
	{ PCIE_0_QSERDES_PLL_DEC_START_MODE0,			0x68 },
	{ PCIE_0_QSERDES_PLL_DEC_START_MODE1,			0x53 },
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START1_MODE0,		0xAB },
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START2_MODE0,		0xAA },
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START3_MODE0,		0x02 },
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START1_MODE1,		0x55 },
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START2_MODE1,		0x55 },
	{ PCIE_0_QSERDES_PLL_DIV_FRAC_START3_MODE1,		0x05 },
	{ PCIE_0_QSERDES_PLL_INTEGLOOP_GAIN0_MODE0,		0xA0 },
	{ PCIE_0_QSERDES_PLL_INTEGLOOP_GAIN0_MODE1,		0xA0 },
	{ PCIE_0_QSERDES_PLL_VCO_TUNE1_MODE0,			0x24 },
	{ PCIE_0_QSERDES_PLL_VCO_TUNE2_MODE0,			0x02 },
	{ PCIE_0_QSERDES_PLL_VCO_TUNE1_MODE1,			0xB4 },
	{ PCIE_0_QSERDES_PLL_VCO_TUNE2_MODE1,			0x03 },
	{ PCIE_0_QSERDES_PLL_CLK_SELECT,			0x32 },
	{ PCIE_0_QSERDES_PLL_HSCLK_SEL,				0x01 },
	{ PCIE_0_QSERDES_PLL_CORE_CLK_EN,			0x00 },
	{ PCIE_0_QSERDES_PLL_CMN_CONFIG,			0x06 },
	{ PCIE_0_QSERDES_PLL_SVS_MODE_CLK_SEL,			0x05 },
	{ PCIE_0_QSERDES_PLL_CORECLK_DIV_MODE1,			0x08 },
	{ PCIE_0_QSERDES_TX0_RES_CODE_LANE_OFFSET_TX,		0x02 },
	{ PCIE_0_QSERDES_TX0_LANE_MODE_1,			0x06 },
	{ PCIE_0_QSERDES_TX0_RCV_DETECT_LVL_2,			0x12 },
	{ PCIE_0_QSERDES_RX0_UCDR_FO_GAIN,			0x0C },
	{ PCIE_0_QSERDES_RX0_UCDR_SO_GAIN,			0x02 },
	{ PCIE_0_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE,	0x7F },
	{ PCIE_0_QSERDES_RX0_UCDR_PI_CONTROLS,			0x70 },
	{ PCIE_0_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2,		0x61 },
	{ PCIE_0_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3,		0x04 },
	{ PCIE_0_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4,		0x1E },
	{ PCIE_0_QSERDES_RX0_RX_IDAC_TSETTLE_LOW,		0xC0 },
	{ PCIE_0_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH,		0x00 },
	{ PCIE_0_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1,	0x73 },
	{ PCIE_0_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2,		0x80 },
	{ PCIE_0_QSERDES_RX0_SIGDET_ENABLES,			0x1C },
	{ PCIE_0_QSERDES_RX0_SIGDET_CNTRL,			0x03 },
	{ PCIE_0_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL,		0x14 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_00_LOW,			0xF0 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH,			0x01 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH2,			0x2F },
	{ PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH3,			0xD3 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_00_HIGH4,			0x40 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_01_LOW,			0x01 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH,			0x02 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH2,			0xC8 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH3,			0x09 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_01_HIGH4,			0xB1 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_10_LOW,			0x00 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH,			0x02 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH2,			0xC8 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH3,			0x09 },
	{ PCIE_0_QSERDES_RX0_RX_MODE_10_HIGH4,			0xB1 },
	{ PCIE_0_QSERDES_RX0_DFE_EN_TIMER,			0x04 },
	{ PCIE_0_PCS_COM_FLL_CNTRL1,				0x01 },
	{ PCIE_0_PCS_COM_REFGEN_REQ_CONFIG1,			0x0D },
	{ PCIE_0_PCS_COM_G12S1_TXDEEMPH_M3P5DB,			0x10 },
	{ PCIE_0_PCS_COM_RX_SIGDET_LVL,				0xAA },
	{ PCIE_0_PCS_COM_P2U3_WAKEUP_DLY_TIME_AUXCLK_L,		0x01 },
	{ PCIE_0_PCS_COM_RX_DCC_CAL_CONFIG,			0x01 },
	{ PCIE_0_PCS_COM_EQ_CONFIG5,				0x01 },
	{ PCIE_0_PCS_PCIE_POWER_STATE_CONFIG2,			0x0D },
	{ PCIE_0_PCS_PCIE_POWER_STATE_CONFIG4,			0x07 },
	{ PCIE_0_PCS_PCIE_ENDPOINT_REFCLK_DRIVE,		0xC1 },
	{ PCIE_0_PCS_PCIE_L1P1_WAKEUP_DLY_TIME_AUXCLK_L,	0x01 },
	{ PCIE_0_PCS_PCIE_L1P2_WAKEUP_DLY_TIME_AUXCLK_L,	0x01 },
	{ PCIE_0_PCS_PCIE_OSC_DTCT_ACTIONS,			0x00 },
	{ PCIE_0_PCS_PCIE_EQ_CONFIG1,				0x11 },
	{ PCIE_0_PCS_PCIE_PRESET_P10_PRE,			0x00 },
	{ PCIE_0_PCS_PCIE_PRESET_P10_POST,			0x58 },
	{ PCIE_0_PCS_COM_SW_RESET,				0x00 },
	{ PCIE_0_PCS_COM_START_CONTROL,				0x03 },
#endif
};

enum pcie_verion{
	PCIE_V0,
	PCIE_V1,
	PCIE_V2,
};

static const struct udevice_id pcie_ver_ids[] = {
	{ .compatible = "qcom,ipq806x-pcie", .data = PCIE_V0 },
	{ .compatible = "qcom,ipq40xx-pcie", .data = PCIE_V1 },
	{ .compatible = "qcom,ipq807x-pcie", .data = PCIE_V2 },
	{ .compatible = "qcom,ipq6018-pcie", .data = PCIE_V2 },
	{ .compatible = "qcom,ipq5018-pcie", .data = PCIE_V2 },
	{ },
};

struct ipq_pcie {
	struct pci_controller hose;
	struct fdt_resource pci_dbi;
	struct fdt_resource parf;
	struct fdt_resource elbi;
	struct fdt_resource dm_iatu;
	struct fdt_resource axi_conf;
	struct fdt_resource axi_bars;
	struct fdt_resource pci_rst;
	struct fdt_resource pci_phy;

	int rst_gpio;
	int is_gen3;
	int linkup;
	int version;
	int skip_phy_init;
};

static void ipq_pcie_write_mask(uint32_t addr,
				uint32_t clear_mask, uint32_t set_mask)
{
	uint32_t val;

	val = (readl(addr) & ~clear_mask) | set_mask;
	writel(val, addr);
}

static void ipq_pcie_parf_reset(uint32_t addr, int domain, int assert)

{
	if (assert)
		ipq_pcie_write_mask(addr, 0, domain);
	else
		ipq_pcie_write_mask(addr, domain, 0);
}

void ipq_pcie_config_cfgtype(uint32_t phyaddr)
{
	uint32_t bdf, cfgtype;

	cfgtype = PCI_CFG0_RDWR;
	bdf = MSM_PCIE_DEV_CFG_ADDR;

	writel(0, phyaddr + PCIE20_PLR_IATU_VIEWPORT);

	/* Program Bdf Address */
	writel(bdf, phyaddr + PCIE20_PLR_IATU_LTAR);

	/* Write Config Request Type */
	writel(cfgtype, phyaddr + PCIE20_PLR_IATU_CTRL1);
}

#define PCIE_ATU_CR1_OUTBOUND_6_GEN3			0xC00
#define PCIE_ATU_CR2_OUTBOUND_6_GEN3			0xC04
#define PCIE_ATU_LOWER_BASE_OUTBOUND_6_GEN3		0xC08
#define PCIE_ATU_UPPER_BASE_OUTBOUND_6_GEN3		0xC0C
#define PCIE_ATU_LIMIT_OUTBOUND_6_GEN3			0xC10
#define PCIE_ATU_LOWER_TARGET_OUTBOUND_6_GEN3		0xC14
#define PCIE_ATU_UPPER_TARGET_OUTBOUND_6_GEN3		0xC18

#define PCIE_ATU_CR1_OUTBOUND_7_GEN3			0xE00
#define PCIE_ATU_CR2_OUTBOUND_7_GEN3			0xE04
#define PCIE_ATU_LOWER_BASE_OUTBOUND_7_GEN3		0xE08
#define PCIE_ATU_UPPER_BASE_OUTBOUND_7_GEN3		0xE0C
#define PCIE_ATU_LIMIT_OUTBOUND_7_GEN3			0xE10
#define PCIE_ATU_LOWER_TARGET_OUTBOUND_7_GEN3		0xE14
#define PCIE_ATU_UPPER_TARGET_OUTBOUND_7_GEN3		0xE18


static void dw_pcie_writel_rc_gen3(uint32_t dbi_base, u32 val, u32 reg)
{
		writel(val, dbi_base + reg);
}


void ipq_pcie_prog_outbound_atu(struct ipq_pcie *pcie)
{

	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x4, PCIE_ATU_CR1_OUTBOUND_6_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x90000000, PCIE_ATU_CR2_OUTBOUND_6_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x0, PCIE_ATU_LOWER_BASE_OUTBOUND_6_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x0, PCIE_ATU_UPPER_BASE_OUTBOUND_6_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x00107FFFF, PCIE_ATU_LIMIT_OUTBOUND_6_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x0, PCIE_ATU_LOWER_TARGET_OUTBOUND_6_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x0, PCIE_ATU_UPPER_TARGET_OUTBOUND_6_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x5, PCIE_ATU_CR1_OUTBOUND_7_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x90000000, PCIE_ATU_CR2_OUTBOUND_7_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x200000, PCIE_ATU_LOWER_BASE_OUTBOUND_7_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x0, PCIE_ATU_UPPER_BASE_OUTBOUND_7_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x7FFFFF, PCIE_ATU_LIMIT_OUTBOUND_7_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x0, PCIE_ATU_LOWER_TARGET_OUTBOUND_7_GEN3);
	dw_pcie_writel_rc_gen3(pcie->dm_iatu.start, 0x0, PCIE_ATU_UPPER_TARGET_OUTBOUND_7_GEN3);

}

int ipq_pcie_rd_conf_byte(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u8 *val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 1))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	*val = ((rd_val & mask) >> (8 * byte_offset));

	return 0;
}

int ipq_pcie_rd_conf_word(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u16 *val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 2))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	*val = ((rd_val & mask) >> (8 * byte_offset));

	return 0;
}
int ipq_pcie_rd_conf_dword(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u32 *val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 4))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	*val = ((rd_val & mask) >> (8 * byte_offset));

	return 0;
}

int ipq_pcie_wr_conf_byte(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u8 val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val, wr_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 1))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	wr_val = (rd_val & ~mask) |((val << (8 * byte_offset)) & mask);
	writel(wr_val, addr + word_offset);

	return 0;
}

int ipq_pcie_wr_conf_word(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u16 val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val, wr_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 2))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	wr_val = (rd_val & ~mask) |((val << (8 * byte_offset)) & mask);
	writel(wr_val, addr + word_offset);

	return 0;
}

int ipq_pcie_wr_conf_dword(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u32 val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val, wr_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 4))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	wr_val = (rd_val & ~mask) |((val << (8 * byte_offset)) & mask);
	writel(wr_val, addr + word_offset);

	return 0;
}

static void ipq_pcie_config_controller(struct ipq_pcie *pcie)
{

	/*
	 * program and enable address translation region 0 (device config
	 * address space); region type config;
	 * axi config address range to device config address range
	 */
	writel(0, pcie->pci_dbi.start + PCIE20_PLR_IATU_VIEWPORT);

	writel(4, pcie->pci_dbi.start + PCIE20_PLR_IATU_CTRL1);
	writel((1 << 31), pcie->pci_dbi.start + PCIE20_PLR_IATU_CTRL2);
	writel(pcie->axi_conf.start , pcie->pci_dbi.start + PCIE20_PLR_IATU_LBAR);
	writel(0, pcie->pci_dbi.start + PCIE20_PLR_IATU_UBAR);
	writel((pcie->axi_conf.start + pcie->axi_conf.end - 1),
				pcie->pci_dbi.start + PCIE20_PLR_IATU_LAR);
	writel(MSM_PCIE_DEV_CFG_ADDR,
				pcie->pci_dbi.start + PCIE20_PLR_IATU_LTAR);
	writel(0, pcie->pci_dbi.start + PCIE20_PLR_IATU_UTAR);

	/*
	 * program and enable address translation region 2 (device resource
	 * address space); region type memory;
	 * axi device bar address range to device bar address range
	 */
	writel(2, pcie->pci_dbi.start + PCIE20_PLR_IATU_VIEWPORT);

	writel(0, pcie->pci_dbi.start + PCIE20_PLR_IATU_CTRL1);
	writel((1 << 31), pcie->pci_dbi.start + PCIE20_PLR_IATU_CTRL2);
	writel(pcie->axi_bars.start, pcie->pci_dbi.start + PCIE20_PLR_IATU_LBAR);
	writel(0, pcie->pci_dbi.start + PCIE20_PLR_IATU_UBAR);
	writel((pcie->axi_bars.start + pcie->axi_bars.end
		- pcie->axi_conf.end - 1), pcie->pci_dbi.start+ PCIE20_PLR_IATU_LAR);
	writel(pcie->axi_bars.start, pcie->pci_dbi.start + PCIE20_PLR_IATU_LTAR);
	writel(0, pcie->pci_dbi.start + PCIE20_PLR_IATU_UTAR);

	/* 1K PCIE buffer setting */
	writel(0x3, pcie->pci_dbi.start + PCIE20_AXI_MSTR_RESP_COMP_CTRL0);
	writel(0x1, pcie->pci_dbi.start + PCIE20_AXI_MSTR_RESP_COMP_CTRL1);
}

void pcie_linkup(struct ipq_pcie *pcie)
{
	int j, val;

	if (pcie->version != PCIE_V2)
	{
		writel(SLV_ADDR_SPACE_SZ, pcie->parf.start + PARF_SLV_ADDR_SPACE_SIZE);
		mdelay(100);
	}

	if (pcie->is_gen3) {
		writel(DEVICE_TYPE_RC, pcie->parf.start + PCIE_PARF_DEVICE_TYPE);
		writel(BYPASS | MSTR_AXI_CLK_EN | AHB_CLK_EN,
			pcie->parf.start + PARF_MHI_CLOCK_RESET_CTRL);
		writel(GEN3_EQUALIZATION_DISABLE | RXEQ_RGRDLESS_RXTS |
			GEN3_ZRXDC_NONCOMPL, pcie->pci_dbi.start + PCIE30_GEN3_RELATED_OFF);
		writel(ECAM_BLOCKER_EN_RANGE2 | MAC_PHY_POWERDOWN_IN_P2_D_MUX_EN
		| ECAM_REMOVE_OFFSET_EN | ECAM_BLOCKER_EN |
		MST_WAKEUP_EN | SLV_WAKEUP_EN | MSTR_ACLK_CGC_DIS
		| SLV_ACLK_CGC_DIS | AUX_PWR_DET |
		CORE_CLK_2AUX_CLK_MUX_DIS | L23_CLK_RMV_DIS,
		pcie->parf.start + PCIE20_PARF_SYS_CTRL);
	}

	writel(0x0, pcie->pci_dbi.start + PCIE_0_PORT_FORCE_REG);
	val = (L1_ENTRANCE_LATENCY(3) |
		L0_ENTRANCE_LATENCY(3) |
		COMMON_CLK_N_FTS(128) |
		ACK_N_FTS(128));
	writel(val, pcie->pci_dbi.start + PCIE_0_ACK_F_ASPM_CTRL_REG);

	val = (FAST_TRAINING_SEQ(128) |
		NUM_OF_LANES(2) |
		DIRECT_SPEED_CHANGE);
	writel(val, pcie->pci_dbi.start + PCIE_0_GEN2_CTRL_REG);
	writel(PCI_TYPE0_BUS_MASTER_EN,
		pcie->pci_dbi.start + PCIE_0_TYPE0_STATUS_COMMAND_REG_1);
	writel(DBI_RO_WR_EN, pcie->pci_dbi.start + PCIE_0_MISC_CONTROL_1_REG);
	writel(0x0002FD7F, pcie->pci_dbi.start + 0x84);

	val = (PCIE_CAP_ASPM_OPT_COMPLIANCE |
		PCIE_CAP_LINK_BW_NOT_CAP |
		PCIE_CAP_DLL_ACTIVE_REP_CAP |
		PCIE_CAP_L1_EXIT_LATENCY(4) |
		PCIE_CAP_L0S_EXIT_LATENCY(4) |
		PCIE_CAP_MAX_LINK_WIDTH(1) |
		PCIE_CAP_MAX_LINK_SPEED(1));
	writel(val, pcie->pci_dbi.start + PCIE_0_LINK_CAPABILITIES_REG);

	writel(PCIE_CAP_CPL_TIMEOUT_DISABLE,
		pcie->pci_dbi.start + PCIE_0_DEVICE_CONTROL2_DEVICE_STATUS2_REG);

	writel(0x10110008, pcie->pci_dbi.start + PCIE_0_TYPE0_LINK_CONTROL_LINK_STATUS_REG_1);

	if (pcie->is_gen3)
		writel(PCIE_CAP_CURR_DEEMPHASIS | SPEED_GEN3,
			pcie->pci_dbi.start + PCIE20_LNK_CONTROL2_LINK_STATUS2);
	writel(LTSSM_EN, pcie->parf.start + PCIE_0_PCIE20_PARF_LTSSM);

	if (pcie->version == PCIE_V2)
		gpio_set_value(pcie->rst_gpio, GPIO_OUT_HIGH);
	mdelay(200);

	for (j = 0; j < 400; j++) {
		val = readl(pcie->pci_dbi.start + PCIE_0_TYPE0_LINK_CONTROL_LINK_STATUS_REG_1);
		if (val & (1 << 29)) {
			printf("PCI Link Intialized\n");
			pcie->linkup = 1;
			break;
		}
		udelay(100);
	}

	if (pcie->is_gen3) {
		writel((pcie->pci_dbi.start + 0x1000), pcie->parf.start + PARF_BLOCK_SLV_AXI_WR_BASE);
		writel((pcie->pci_dbi.start + 0x100000), pcie->parf.start + PARF_BLOCK_SLV_AXI_WR_LIMIT);
		writel((pcie->pci_dbi.start + 0x1000), pcie->parf.start + PARF_BLOCK_SLV_AXI_RD_BASE);
		writel((pcie->pci_dbi.start + 0x100000), pcie->parf.start + PARF_BLOCK_SLV_AXI_RD_LIMIT);
		writel((pcie->pci_dbi.start), pcie->parf.start + PARF_ECAM_BASE);
		writel((pcie->pci_dbi.start + 0x1000), pcie->parf.start + PARF_ECAM_OFFSET_REMOVAL_BASE);
		writel((pcie->pci_dbi.start + 0x200000), pcie->parf.start + PARF_ECAM_OFFSET_REMOVAL_LIMIT);
		writel((pcie->pci_dbi.start + 0x108000), pcie->parf.start + PARF_BLOCK_SLV_AXI_WR_BASE_2);
		writel((pcie->pci_dbi.start + 0x200000), pcie->parf.start + PARF_BLOCK_SLV_AXI_WR_LIMIT_2);
		writel((pcie->pci_dbi.start + 0x108000), pcie->parf.start + PARF_BLOCK_SLV_AXI_RD_BASE_2);
		writel((pcie->pci_dbi.start + 0x200000), pcie->parf.start + PARF_BLOCK_SLV_AXI_RD_LIMIT_2);
		ipq_pcie_prog_outbound_atu(pcie);
	} else {
		ipq_pcie_config_controller(pcie);
	}
}

void pcie_v0_linkup(struct ipq_pcie *pcie, int id)
{
	int j;
	uint32_t val;
	/* assert PCIe PARF reset while powering the core */
	ipq_pcie_parf_reset(pcie->pci_rst.start, BIT(6), 0);

	ipq_pcie_parf_reset(pcie->pci_rst.start, BIT(2), 1);
	board_pcie_clock_init(id);
	/*
	 * de-assert PCIe PARF reset;
	 * wait 1us before accessing PARF registers
	 */
	ipq_pcie_parf_reset(pcie->pci_rst.start, BIT(2), 0);
	udelay(1);

	/* enable PCIe clocks and resets */
	val = (readl(pcie->parf.start + PCIE20_PARF_PHY_CTRL) & ~BIT(0));
	writel(val, pcie->parf.start + PCIE20_PARF_PHY_CTRL);

	ipq_pcie_write_mask(pcie->parf.start + PCIE20_PARF_PHY_CTRL,
				PHY_CTRL_PHY_TX0_TERM_OFFSET_MASK,
				PHY_CTRL_PHY_TX0_TERM_OFFSET(0));

	/* PARF programming */
	writel(PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN1(0x18) |
			PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_3_5DB(0x18) |
			PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_6DB(0x22),
			pcie->parf.start + PCIE20_PARF_PCS_DEEMPH);

	writel(PCIE20_PARF_PCS_SWING_TX_SWING_FULL(0x78) |
			PCIE20_PARF_PCS_SWING_TX_SWING_LOW(0x78),
			pcie->parf.start + PCIE20_PARF_PCS_SWING);

	writel((4<<24), pcie->parf.start + PCIE20_PARF_CONFIG_BITS);

	ipq_pcie_write_mask(pcie->parf.start + PCIE20_PARF_PHY_REFCLK,
				REF_USE_PAD, REF_SSP_EN);

	/* enable access to PCIe slave port on system fabric */
	if (id == 0) {
		writel(BIT(4), PCIE_SFAB_AXI_S5_FCLK_CTL);
	}

	udelay(1);
	/* de-assert PICe PHY, Core, POR and AXI clk domain resets */
	ipq_pcie_parf_reset(pcie->pci_rst.start, BIT(5), 0);
	ipq_pcie_parf_reset(pcie->pci_rst.start, BIT(4), 0);
	ipq_pcie_parf_reset(pcie->pci_rst.start, BIT(3), 0);
	ipq_pcie_parf_reset(pcie->pci_rst.start, BIT(0), 0);

	/* enable link training */
	ipq_pcie_write_mask( pcie->elbi.start + PCIE20_ELBI_SYS_CTRL, 0,
			BIT(0));
	udelay(500);

	for (j = 0; j < 10; j++) {
		val = readl(pcie->pci_dbi.start +
				PCIE_0_TYPE0_LINK_CONTROL_LINK_STATUS_REG_1);
		if (val & BIT(29)) {
			printf("PCI%d Link Intialized\n", id);
			pcie->linkup = 1;
			break;
		}
		udelay(10000);
	}
	ipq_pcie_config_controller(pcie);

}

static int ipq_pcie_parse_dt(const void *fdt, int id,
			       struct ipq_pcie *pcie)
{
	int err, rst_gpio, node;
	char name[16];

	snprintf(name, sizeof(name), "pci%d", id);
	node = fdt_path_offset(fdt, name);
	if (node < 0) {
		printf("PCI%d is not defined in the device tree\n", id);
		return node;
	}

	err = fdt_get_named_resource(fdt, node, "reg", "reg-names", "pci_dbi",
				     &pcie->pci_dbi);
	if (err < 0) {
		error("resource \"pads\" not found");
		return err;
	}

	err = fdt_get_named_resource(fdt, node, "reg", "reg-names", "parf",
				     &pcie->parf);
	if (err < 0) {
		error("resource \"afi\" not found");
		return err;
	}

	err = fdt_get_named_resource(fdt, node, "reg", "reg-names", "elbi",
				     &pcie->elbi);
	if (err < 0) {
		error("resource \"cs\" not found");
		return err;
	}

	err = fdt_get_named_resource(fdt, node, "reg", "reg-names", "axi_bars",
				     &pcie->axi_bars);
	if (err < 0) {
		error("resource \"cs\" not found");
		return err;
	}

	err = fdt_get_named_resource(fdt, node, "reg", "reg-names", "axi_conf",
				     &pcie->axi_conf);
	if (err < 0) {
		error("resource \"cs\" not found");
		return err;
	}

	err = fdt_get_named_resource(fdt, node, "reg", "reg-names", "pci_rst",
				     &pcie->pci_rst);
	if (err < 0) {
		error("resource \"cs\" not found");
		return err;
	}

	pcie->is_gen3 = 0;
	if(pcie->version == PCIE_V2) {
		pcie->is_gen3 = fdtdec_get_int(fdt, node, "gen3", 0);
		err = fdt_get_named_resource(fdt, node, "reg", "reg-names", "pci_phy",
					     &pcie->pci_phy);
		if (err < 0) {
			uint32_t soc_ver_major, soc_ver_minor;
			int ret;

			ret = get_soc_version(&soc_ver_major, &soc_ver_minor);
			if (!ret) {
				if(soc_ver_major == 1) {
					err = fdt_get_named_resource(fdt, node, "reg", "reg-names", "pci_phy_gen2",
							&pcie->pci_phy);
					if (err < 0)
						goto err;
					pcie->is_gen3 = 0;
				} else if(soc_ver_major == 2) {
					err = fdt_get_named_resource(fdt, node, "reg", "reg-names", "pci_phy_gen3",
							&pcie->pci_phy);
					if (err < 0)
						goto err;
					pcie->is_gen3 = 1;
				}
			} else {
				goto err;
			}
		}
		pcie->skip_phy_init =
				fdtdec_get_int(fdt, node, "skip_phy_int", 0);
	}
	rst_gpio = fdtdec_get_int(fdt, node, "perst_gpio", 0);
	if (rst_gpio <= 0) {
		debug("PCI: Can't get perst_gpio\n");
		return -1;
	}
	pcie->rst_gpio = rst_gpio;

	if (pcie->is_gen3) {
		err = fdt_get_named_resource(fdt, node, "reg", "reg-names", "dm_iatu",
				&pcie->dm_iatu);
		if (err < 0) {
			error("resource \"dm_iatu\" not found");
			return err;
		}
	}

	return 0;
err:
	error("resource \"phy\" not found");
	return err;
}

void pci_controller_init_v1(struct ipq_pcie *pcie)
{
	uint32_t val;

	/* Assert cc_pcie20_mstr_axi_ares */
	val = readl(pcie->pci_rst.start);
	val |= PCIE_RST_CTRL_AXI_M_ARES;
	writel(val, pcie->pci_rst.start);

	/* Assert cc_pcie20_slv_axi_ares */
	val = readl(pcie->pci_rst.start);
	val |= PCIE_RST_CTRL_AXI_S_ARES;
	writel(val, pcie->pci_rst.start);

	/* Assert cc_pcie20_core_ares */
	writel(PCIE_RST_CTRL_PIPE_ARES, pcie->pci_rst.start);

	/* Assert cc_pcie20_core_sticky_area */
	val = readl(pcie->pci_rst.start);
	val |= PCIE_RST_CTRL_PIPE_STICKY_ARES;
	writel(val, pcie->pci_rst.start);

	/* Assert cc_pcie20_phy_ahb_ares */
	val = readl(pcie->pci_rst.start);
	val |= PCIE_RST_CTRL_PIPE_PHY_AHB_ARES;
	writel(val, pcie->pci_rst.start);

	/* Assert cc_pcie20_mstr_sticky_ares */
	val = readl(pcie->pci_rst.start);
	val |= PCIE_RST_CTRL_AXI_M_STICKY_ARES;
	writel(val, pcie->pci_rst.start);

	gpio_set_value(pcie->rst_gpio, GPIO_OUT_LOW);

	/* Assert cc_pcie20_ahb_ares;  */
	val = readl(pcie->pci_rst.start);
	val |= PCIE_RST_CTRL_AHB_ARES;
	writel(val, pcie->pci_rst.start);

	/* DeAssert cc_pcie20_ahb_ares */
	val = readl(pcie->pci_rst.start);
	val &= ~(PCIE_RST_CTRL_PIPE_PHY_AHB_ARES);
	writel(val, pcie->pci_rst.start);

	/* DeAssert cc_pcie20_pciephy_phy_ares*/
	val = readl(pcie->pci_rst.start);
	val &= ~(PCIE_RST_CTRL_PIPE_ARES);
	writel(val, pcie->pci_rst.start);

	/* DeAssert cc_pcie20_core_sticky_ares */
	val = readl(pcie->pci_rst.start);
	val &= ~(PCIE_RST_CTRL_PIPE_STICKY_ARES);
	writel(val, pcie->pci_rst.start);

	mdelay(5);

	gpio_set_value(pcie->rst_gpio, GPIO_OUT_HIGH);

	/* DeAssert cc_pcie20_mstr_axi_ares */
	val = readl(pcie->pci_rst.start);
	val &= ~(PCIE_RST_CTRL_AXI_M_ARES);
	writel(val, pcie->pci_rst.start);

	/* DeAssert cc_pcie20_mstr_axi_ares */
	val = readl(pcie->pci_rst.start);
	val &= ~(PCIE_RST_CTRL_AXI_M_STICKY_ARES);
	writel(val, pcie->pci_rst.start);

	/* DeAssert cc_pcie20_slv_axi_ares */
	val = readl(pcie->pci_rst.start);
	val &= ~(PCIE_RST_CTRL_AXI_S_ARES);
	writel(val, pcie->pci_rst.start);

	/* DeAssert cc_pcie20_phy_ahb_ares  */
	val = readl(pcie->pci_rst.start);
	val &= ~(PCIE_RST_CTRL_AHB_ARES);
	writel(val, pcie->pci_rst.start);
}

static const struct phy_regs pcie_phy_regs[] = {
#if !defined(CONFIG_IPQ6018)
	{ PCIE_USB3_PCS_POWER_DOWN_CONTROL,			0x00000003 },
	{ QSERDES_COM_BIAS_EN_CLKBUFLR_EN,			0x00000018 },
	{ QSERDES_COM_CLK_ENABLE1,				0x00000010 },
	{ QSERDES_COM_BG_TRIM,					0x0000000f },
	{ QSERDES_COM_LOCK_CMP_EN,				0x00000001 },
	{ QSERDES_COM_VCO_TUNE_MAP,				0x00000000 },
	{ QSERDES_COM_VCO_TUNE_TIMER1,				0x000000ff },
	{ QSERDES_COM_VCO_TUNE_TIMER2,				0x0000001f },
	{ QSERDES_COM_CMN_CONFIG,				0x00000006 },
	{ PCIE_QSERDES_COM_PLL_IVCO,				0x0000000f },
	{ QSERDES_COM_HSCLK_SEL,				0x00000000 },
	{ PCIE_QSERDES_COM_SVS_MODE_CLK_SEL,			0x00000001 },
	{ PCIE_QSERDES_COM_CORE_CLK_EN,				0x00000020 },
	{ QSERDES_COM_CORECLK_DIV,				0x0000000a },
	{ QSERDES_COM_RESETSM_CNTRL,				0x00000020 },
	{ PCIE_QSERDES_COM_BG_TIMER,				0x00000009 },
	{ PCIE_QSERDES_COM_SYSCLK_EN_SEL,			0x0000000a },
	{ PCIE_QSERDES_COM_DEC_START_MODE0,			0x00000082 },
	{ PCIE_QSERDES_COM_DIV_FRAC_START3_MODE0,		0x00000003 },
	{ PCIE_QSERDES_COM_DIV_FRAC_START2_MODE0,		0x00000055 },
	{ PCIE_QSERDES_COM_DIV_FRAC_START1_MODE0,		0x00000055 },
	{ QSERDES_COM_LOCK_CMP3_MODE0,				0x00000000 },
	{ QSERDES_COM_LOCK_CMP2_MODE0,				0x0000000D },
	{ PCIE_QSERDES_COM_LOCK_CMP1_MODE0,			0x00000D04 },
	{ QSERDES_COM_CLK_SELECT,				0x00000033 },
	{ PCIE_QSERDES_COM_SYS_CLK_CTRL,			0x00000002 },
	{ QSERDES_COM_SYSCLK_BUF_ENABLE,			0x0000001f },
	{ QSERDES_COM_CP_CTRL_MODE0,				0x0000000b },
	{ QSERDES_COM_PLL_RCTRL_MODE0,				0x00000016 },
	{ QSERDES_COM_PLL_CCTRL_MODE0,				0x00000028 },
	{ QSERDES_COM_INTEGLOOP_GAIN1_MODE0,			0x00000000 },
	{ QSERDES_COM_INTEGLOOP_GAIN0_MODE0,			0x00000080 },
	{ QSERDES_COM_BIAS_EN_CTRL_BY_PSM,			0x00000001 },
	{ QSERDES_COM_VCO_TUNE_CTRL,				0x0000000a },
	{ QSERDES_COM_SSC_EN_CENTER,				0x00000001 },
	{ PCIE_QSERDES_COM_SSC_PER1,				0x00000031 },
	{ QSERDES_COM_SSC_PER2,					0x00000001 },
	{ QSERDES_COM_SSC_ADJ_PER1,				0x00000002 },
	{ QSERDES_COM_SSC_ADJ_PER2,				0x00000000 },
	{ QSERDES_COM_SSC_STEP_SIZE1,				0x0000002f },
	{ QSERDES_COM_SSC_STEP_SIZE2,				0x00000019 },
	{ QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN,		0x00000045 },
	{ QSERDES_TX_LANE_MODE,					0x00000006 },
	{ QSERDES_TX_RES_CODE_LANE_OFFSET,			0x00000002 },
	{ PCIE_QSERDES_TX_RCV_DETECT_LVL_2,			0x00000012 },
	{ QSERDES_RX_SIGDET_ENABLES,				0x0000001c },
	{ PCIE_QSERDES_RX_SIGDET_DEGLITCH_CNTRL,		0x00000014 },
	{ PCIE_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2,		0x00000001 },
	{ PCIE_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3,		0x00000000 },
	{ PCIE_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4,		0x000000db },
	{ QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE,		0x0000004b },
	{ PCIE_QSERDES_RX_UCDR_SO_GAIN,				0x00000004 },
	{ QSERDES_RX_UCDR_SO_GAIN_HALF,				0x00000004 },
	{ QSERDES_COM_CLK_EP_DIV,				0x00000019 },
	{ PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE,			0x00000004 },
	{ PCIE_USB3_PCS_OSC_DTCT_ACTIONS,			0x00000000 },
	{ PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK,		0x00000040 },
	{ PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_MSB,	0x00000000 },
	{ PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_LSB,	0x00000040 },
	{ PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME_AUXCLK_LSB,	0x00000000 },
	{ PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK,		0x00000040 },
	{ PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME,			0x00000073 },
	{ QSERDES_RX_SIGDET_CNTRL,				0x00000007 },
	{ PCIE_USB3_PCS_RX_SIGDET_LVL,				0x00000099 },
	{ PCIE_USB3_PCS_TXDEEMPH_M6DB_V0,			0x00000015 },
	{ PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V0,			0x0000000e },
	{ PCIE_USB3_PCS_SW_RESET,				0x00000000 },
	{ PCIE_USB3_PCS_START_CONTROL,				0x00000003 },
#endif
};

void pcie_phy_init(struct ipq_pcie *pcie)
{
	int i;
	const struct phy_regs *regs = pcie_phy_regs;

	if (!phy_initialised) {
		writel(0x10000000, pcie->parf.start + 0x358);
		writel(0x10000000, pcie->parf.start + 0x8358);
		mdelay(100);
		phy_initialised = 1;
	}
	for (i = 0; i < ARRAY_SIZE(pcie_phy_regs); i++)
		writel(regs[i].val, pcie->pci_phy.start + regs[i].reg_offset);

}

static inline void qca_pcie_write_reg(u32 base, u32 offset, u32 value)
{
        writel(value, base + offset);
}

void pcie_phy_v2_init(struct ipq_pcie *pcie)
{
	if (!pcie->skip_phy_init) {
		const struct phy_regs *regs = pcie_phy_v2_init_seq_ipq;
		int i, size = ARRAY_SIZE(pcie_phy_v2_init_seq_ipq);

		for (i = 0; i < size; i++) {
			if (regs[i].reg_offset == PCIE_PHY_DELAY_MS)
				mdelay(regs[i].val);
			else
				writel(regs[i].val, pcie->pci_phy.start + regs[i].reg_offset);
		}
		mdelay(5);
	}
	return;
}

static int pci_ipq_ofdata_to_platdata(int id, struct ipq_pcie *pcie)
{

	if (ipq_pcie_parse_dt(gd->fdt_blob, id, pcie))
		return -EINVAL;

	board_pci_init(id);
	switch(pcie->version) {
		case PCIE_V0:
			pcie_v0_linkup(pcie, id);
			break;
		case PCIE_V1:
			pci_controller_init_v1(pcie);
			pcie_linkup(pcie);
			break;
		case PCIE_V2:
			gpio_set_value(pcie->rst_gpio, GPIO_OUT_LOW);
			if (pcie->is_gen3)
				pcie_phy_v2_init(pcie);
			else
				pcie_phy_init(pcie);
			pcie_linkup(pcie);
			break;
		default:
			break;
	}

	return 0;
}

__weak void ipq_wifi_pci_power_enable(void)
{
	return;
}

void pci_init_board (void)
{
	struct ipq_pcie *pcie;
	int i, bus = 0, ret;
	const struct udevice_id *of_match = pcie_ver_ids;

	pcie = malloc(sizeof(*pcie));
	if (pcie == NULL) {
		printf("PCI: Init failed. Could't allocate memory\n");
		return;
	}

	while (of_match->compatible) {
		ret = fdt_node_offset_by_compatible(gd->fdt_blob, 0,
						of_match->compatible);
		if (ret < 0) {
			of_match++;
			continue;
		}
		pcie->version = of_match->data;
		break;
	}

	ipq_wifi_pci_power_enable();
	for (i = 0; i < PCI_MAX_DEVICES; i++) {
		pcie->linkup = 0;
		pci_ipq_ofdata_to_platdata(i, pcie);
		if (pcie->linkup) {
			pci_hose[i].first_busno = bus;
			pci_hose[i].last_busno = 0xff;
			local_buses[0] = pci_hose[i].first_busno;

			/* PCI memory space */
			pci_set_region (pci_hose[i].regions + 0,
					pcie->pci_dbi.start,
					pcie->pci_dbi.start,
					PCIE20_SIZE, PCI_REGION_MEM);

			/* PCI device confgiuration  space */
			pci_set_region (pci_hose[i].regions + 1,
					pcie->axi_conf.start,
					pcie->axi_conf.start,
				(PCIE_AXI_CONF_SIZE - 1), PCI_REGION_MEM);

			pci_hose[i].region_count = 2;
			pci_register_hose (&pci_hose[i]);
			pci_set_ops (&pci_hose[i],
				ipq_pcie_rd_conf_byte,
				ipq_pcie_rd_conf_word,
				ipq_pcie_rd_conf_dword,
				ipq_pcie_wr_conf_byte,
				ipq_pcie_wr_conf_word,
				ipq_pcie_wr_conf_dword);

			pci_hose[i].last_busno = pci_hose[i].first_busno + 1;
			bus = pci_hose[i].last_busno + 1;
		}
	}
}
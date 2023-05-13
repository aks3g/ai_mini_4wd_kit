/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2015-2016 by 3rd Designing Center
 *              Imageing System Development Division RICOH COMPANY, LTD.
 *  Copyright (C) 2017-2020 by TOPPERS PROJECT Educational Working Group.
 *
 *  �嵭����Ԥϡ��ʲ��� (1)��(4) �ξ�狼��Free Software Foundation 
 *  �ˤ�äƸ�ɽ����Ƥ��� GNU General Public License �� Version 2 �˵�
 *  �Ҥ���Ƥ���������������˸¤ꡤ�ܥ��եȥ��������ܥ��եȥ�����
 *  ����Ѥ�����Τ�ޤࡥ�ʲ�Ʊ���ˤ���ѡ�ʣ�������ѡ������ۡʰʲ���
 *  ���ѤȸƤ֡ˤ��뤳�Ȥ�̵���ǵ������롥
 *  (1) �ܥ��եȥ������򥽡��������ɤη������Ѥ�����ˤϡ��嵭������
 *      ��ɽ�����������Ѿ�浪��Ӳ�����̵�ݾڵ��꤬�����Τޤޤη��ǥ���
 *      ����������˴ޤޤ�Ƥ��뤳�ȡ�
 *  (2) �ܥ��եȥ������򡤥饤�֥������ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ�����Ǻ����ۤ�����ˤϡ������ۤ�ȼ���ɥ�����ȡ�����
 *      �ԥޥ˥奢��ʤɡˤˡ��嵭�����ɽ�����������Ѿ�浪��Ӳ���
 *      ��̵�ݾڵ����Ǻܤ��뤳�ȡ�
 *  (3) �ܥ��եȥ������򡤵�����Ȥ߹���ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ��ʤ����Ǻ����ۤ�����ˤϡ����Τ����줫�ξ�����������
 *      �ȡ�
 *    (a) �����ۤ�ȼ���ɥ�����ȡ����Ѽԥޥ˥奢��ʤɡˤˡ��嵭����
 *        �ɽ�����������Ѿ�浪��Ӳ�����̵�ݾڵ����Ǻܤ��뤳�ȡ�
 *    (b) �����ۤη��֤��̤�������ˡ�ˤ�äơ�TOPPERS�ץ������Ȥ�
 *        ��𤹤뤳�ȡ�
 *  (4) �ܥ��եȥ����������Ѥˤ��ľ��Ū�ޤ��ϴ���Ū�������뤤���ʤ�»
 *      ������⡤�嵭����Ԥ����TOPPERS�ץ������Ȥ����դ��뤳�ȡ�
 *
 *  �ܥ��եȥ������ϡ�̵�ݾڤ��󶡤���Ƥ����ΤǤ��롥�嵭����Ԥ�
 *  ���TOPPERS�ץ������Ȥϡ��ܥ��եȥ������˴ؤ��ơ�����Ŭ�Ѳ�ǽ����
 *  �ޤ�ơ������ʤ��ݾڤ�Ԥ�ʤ����ޤ����ܥ��եȥ����������Ѥˤ��ľ
 *  ��Ū�ޤ��ϴ���Ū�������������ʤ�»���˴ؤ��Ƥ⡤������Ǥ�����ʤ���
 * 
 *  @(#) $Id: cmsis.h,v 1.1 2020/07/30 05:59:30 roi Exp $
 */

#ifndef _CMSIS_M4_H_
#define _CMSIS_M4_H_

#define __NVIC_PRIO_BITS          4       /* STM32M4XX uses 4 Bits for the Priority Levels */

/* Memory mapping of Cortex-M4 Hardware */
#define TADR_ITM_BASE       (0xE0000000UL)	/* ITM Base Address */
  #define TOFF_ITM_PORT     0x0000		/* (W)  ITM Stimulus Port Registers */
  #define TOFF_ITM_TER      0x0E00		/* (RW) ITM Trace Enable Register */
  #define TOFF_ITM_TPR      0x0E40		/* (RW) ITM Trace Privilege Register */
  #define TOFF_ITM_TCR      0x0E80		/* (RW) ITM Trace Control Register */
#define TADR_SCS_BASE       (0xE000E000UL)	/* System Control Space Base Address  */
  #define TOFF_SCS_ICTR     0x0004		/* (R)  Interrupt Controller Type Register */
  #define TOFF_SCS_ACTLR    0x0008		/* (RW) Auxiliary Control Register */
#define TADR_COREDEBUG_BASE (0xE000EDF0UL)				/* Core Debug Base Address */
#define TADR_SYSTICK_BASE   (TADR_SCS_BASE+0x0010UL)	/* SysTick Base Address */
  #define TOFF_SYSTICK_CTRL 0x0000		/* (RW) SysTick Control and Status Register */
  #define TOFF_SYSTICK_LOAD 0x0004		/* (RW) SysTick Reload Value Register */
  #define TOFF_SYSTICK_VAL  0x0008		/* (RW) SysTick Current Value Register */
  #define TOFF_SYSTICK_CALIB 0x000C		/* (R)  SysTick Calibration Register */
#define TADR_NVIC_BASE      (TADR_SCS_BASE+0x0100UL)	/* NVIC Base Address */
  #define TOFF_NVIC_ISER    0x0000		/* (RW) Interrupt Set Enable Register */
  #define TOFF_NVIC_ICER    0x0080		/* (RW) Interrupt Clear Enable Register */
  #define TOFF_NVIC_ISPR    0x0100		/* (RW) Interrupt Set Pending Register */
  #define TOFF_NVIC_ICPR    0x0180		/* (RW) Interrupt Clear Pending Register */
  #define TOFF_NVIC_IABR    0x0200		/* (RW) Interrupt Active bit Register */
  #define TOFF_NVIC_IP      0x0300		/* (RW) Interrupt Priority Register (8Bit wide) */
  #define TOFF_NVIC_STIR    0x0E00		/* (W)  Software Trigger Interrupt Register */
#define TADR_SCB_BASE       (TADR_SCS_BASE+0x0D00UL)	/* System Control Block Base Address  */
  #define TOFF_SCB_CPUID    0x0000		/* (R)  CPUID Base Register */
  #define TOFF_SCB_ICSR     0x0004		/* (RW) Interrupt Control and State Register */
  #define TOFF_SCB_VTOR     0x0008		/* (RW) Vector Table Offset Register */
  #define TOFF_SCB_AIRCR    0x000C		/* (RW) Application Interrupt and Reset Control Register */
    #define SCB_AIRCR_VECTKEY_Pos   16	/* SCB AIRCR: VECTKEY Position */
    #define SCB_AIRCR_VECTKEY_Msk   (0xFFFF << SCB_AIRCR_VECTKEY_Pos)	/* SCB AIRCR: VECTKEY Mask */
    #define SCB_AIRCR_ENDIANESS_Pos 15	/* SCB AIRCR: ENDIANESS Position */
    #define SCB_AIRCR_ENDIANESS_Msk (1 << SCB_AIRCR_ENDIANESS_Pos)	/* SCB AIRCR: ENDIANESS Mask */
    #define SCB_AIRCR_PRIGROUP_Pos  8	/* SCB AIRCR: PRIGROUP Position */
    #define SCB_AIRCR_PRIGROUP_Msk  (7 << SCB_AIRCR_PRIGROUP_Pos)	/* SCB AIRCR: PRIGROUP Mask */
    #define SCB_AIRCR_SYSRESETREQ_Pos 2	/* SCB AIRCR: SYSRESETREQ Position */
    #define SCB_AIRCR_SYSRESETREQ_Msk (1 << SCB_AIRCR_SYSRESETREQ_Pos)	/* SCB AIRCR: SYSRESETREQ Mask */
    #define SCB_AIRCR_VECTCLRACTIVE_Pos 1	/* SCB AIRCR: VECTCLRACTIVE Position */
    #define SCB_AIRCR_VECTCLRACTIVE_Msk (1 << SCB_AIRCR_VECTCLRACTIVE_Pos)	/* SCB AIRCR: VECTCLRACTIVE Mask */
    #define SCB_AIRCR_VECTRESET_Pos 0	/* SCB AIRCR: VECTRESET Position */
    #define SCB_AIRCR_VECTRESET_Msk (1)		/* SCB AIRCR: VECTRESET Mask */
  #define TOFF_SCB_SCR      0x0010		/* (RW) System Control Register */
    #define SCB_SCR_SEVONPEND_Pos   4	/* SCB SCR: SEVONPEND Position */
    #define SCB_SCR_SEVONPEND_Msk   (1 << SCB_SCR_SEVONPEND_Pos)	/* SCB SCR: SEVONPEND Mask */
    #define SCB_SCR_SLEEPDEEP_Pos   2	/* SCB SCR: SLEEPDEEP Position */
    #define SCB_SCR_SLEEPDEEP_Msk   (1 << SCB_SCR_SLEEPDEEP_Pos)	/* SCB SCR: SLEEPDEEP Mask */
    #define SCB_SCR_SLEEPONEXIT_Pos 1	/* SCB SCR: SLEEPONEXIT Position */
    #define SCB_SCR_SLEEPONEXIT_Msk (1 << SCB_SCR_SLEEPONEXIT_Pos)	/* SCB SCR: SLEEPONEXIT Mask */
  #define TOFF_SCB_CCR      0x0014		/* (RW) Configuration Control Register */
  #define TOFF_SCB_SHP4     0x0018		/* (RW-8) System Handlers Priority Registers 4 */
  #define TOFF_SCB_SHP5     0x0019		/* (RW-8) System Handlers Priority Registers 5 */
  #define TOFF_SCB_SHP6     0x001A		/* (RW-8) System Handlers Priority Registers 6 */
  #define TOFF_SCB_SHP7     0x001B		/* (RW-8) System Handlers Priority Registers 7 */
  #define TOFF_SCB_SHP8     0x001C		/* (RW-8) System Handlers Priority Registers 8 */
  #define TOFF_SCB_SHP9     0x001D		/* (RW-8) System Handlers Priority Registers 9 */
  #define TOFF_SCB_SHP10    0x001E		/* (RW-8) System Handlers Priority Registers 10 */
  #define TOFF_SCB_SHP11    0x001F		/* (RW-8) System Handlers Priority Registers 11 */
  #define TOFF_SCB_SHP12    0x0020		/* (RW-8) System Handlers Priority Registers 12 */
  #define TOFF_SCB_SHP13    0x0021		/* (RW-8) System Handlers Priority Registers 13 */
  #define TOFF_SCB_SHP14    0x0022		/* (RW-8) System Handlers Priority Registers 14 */
  #define TOFF_SCB_SHP15    0x0023		/* (RW-8) System Handlers Priority Registers 15 */
  #define TOFF_SCB_SHCSR    0x0024		/* (RW) System Handler Control and State Register */
  #define TOFF_SCB_CFSR     0x0028		/* (RW) Configurable Fault Status Register */
  #define TOFF_SCB_HFSR     0x002C		/* (RW) HardFault Status Register */
  #define TOFF_SCB_DFSR     0x0030		/* (RW) Debug Fault Status Register */
  #define TOFF_SCB_MMFAR    0x0034		/* (RW) MemManage Fault Address Register */
  #define TOFF_SCB_BFAR     0x0038		/* (RW) BusFault Address Register */
  #define TOFF_SCB_AFSR     0x003C		/* (RW) Auxiliary Fault Status Register */
  #define TOFF_SCB_PFR0     0x0040		/* (R)  Processor Feature Register 0 */
  #define TOFF_SCB_PFR1     0x0044		/* (R)  Processor Feature Register 1 */
  #define TOFF_SCB_DFR      0x0048		/* (R)  Debug Feature Register */
  #define TOFF_SCB_ADR      0x004C		/* (R)  Auxiliary Feature Register */
  #define TOFF_SCB_MMFR0    0x0050		/* (R)  Memory Model Feature Register 0 */
  #define TOFF_SCB_MMFR1    0x0054		/* (R)  Memory Model Feature Register 1 */
  #define TOFF_SCB_MMFR2    0x0058		/* (R)  Memory Model Feature Register 2 */
  #define TOFF_SCB_MMFR3    0x005C		/* (R)  Memory Model Feature Register 3 */
  #define TOFF_SCB_ISAR0    0x0060		/* (R)  Instruction Set Attributes Register 0 */
  #define TOFF_SCB_ISAR1    0x0064		/* (R)  Instruction Set Attributes Register 1 */
  #define TOFF_SCB_ISAR2    0x0068		/* (R)  Instruction Set Attributes Register 2 */
  #define TOFF_SCB_ISAR3    0x006C		/* (R)  Instruction Set Attributes Register 3 */
  #define TOFF_SCB_ISAR4    0x0070		/* (R)  Instruction Set Attributes Register 4 */
  #define TOFF_SCB_CPACR    0x0088		/* (RW) Coprocessor Access Control Register */

#endif  /* _CMSIS_M4_H_ */


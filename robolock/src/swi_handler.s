@/*****************************************************************************
@*   swi_handler.s: SWI handler file for NXP LPC230x Family Microprocessors
@*
@*   Copyright(C) 2006, NXP Semiconductor
@*   All rights reserved.
@*
@*   History
@*   2006.09.01  ver 1.00    Prelimnary version, first Release
@*
@*****************************************************************************/

@ modified by Martin Thomas to extrace SWI-number from
@ SWI instruction parameter

// PRESERVE8

.equ SWI_IRQ_DIS,  0
.equ SWI_IRQ_EN,   1
.equ SWI_FIQ_DIS,  2
.equ SWI_FIQ_EN,   3

.equ I_Bit,        0x80
.equ F_Bit,        0x40

@The exported labels and functions
	.global SoftwareInterrupt
	.global IntEnable
	.global IntDisable

.arm
.section .SWI_HANDLER, "ax"

@********************************************************************
@*  SWI interrupt handler											*
@*  Function : SoftwareInterrupt(SWI_Number)						*
@*  The SWI_Number is 0 through 3									*
@*  See below "SwiFunction" table below								*
@*  Parameters:    None												*
@*  input  :       SWI_Number										*
@*  output :       None												*
@*  mthomas: Expects that SWI instruction is called from ARM-mode   *
@*           (not thumb). This is the case if the functions below  *
@*           (IntEnable/IntDisable) get used.
@********************************************************************
SoftwareInterrupt:
	LDR		R0, [LR, #-4]          /* get swi instruction code (ARM-mode) */
	BIC		R0, R0, #0xff000000    /* clear top 8 bits leaving swi "comment field"=number */
	CMP		R0, #4                 /* range check */
	LDRLO	PC, [PC, R0, LSL #2]   /* get jump-address from table */
	MOVS	PC, LR                 /* if out of range: do nothing and return */

SwiFunction:
.word IRQDisable
.word IRQEnable
.word FIQDisable
.word FIQEnable

IRQDisable:
	MRS		R0, SPSR
	ORR		R0, R0, #I_Bit
	MSR		SPSR_c, R0
	MOVS	PC, LR

IRQEnable:
	MRS		R0, SPSR
	BIC		R0, R0, #I_Bit
	MSR		SPSR_c, R0
	MOVS	PC, LR

FIQDisable:
	MRS		R0, SPSR
	ORR		R0, R0, #F_Bit
	MSR		SPSR_c, R0
	MOVS	PC, LR

FIQEnable:
	MRS		R0, SPSR
	BIC		R0, R0, #F_Bit
	MSR		SPSR_c, R0
	MOVS	PC, LR

@**********************************************************************
@*  Call SWI to enable IRQ                                            *
@*  Function : void IntEnable(void)                                   *
@*  Parameters:      None                                             *
@*  input  :         None                                             *
@*  output :         None                                             *
@**********************************************************************
IntEnable:
	SWI     SWI_IRQ_EN
	BX      lr
@ end of IntEnable

@**********************************************************************
@*  Call SWI to disable IRQ                                           *
@*  Function : void IntDisable(void)                                  *
@*  Parameters     : None                                             *
@*  input          : None                                             *
@*  output         : None                                             *
@**********************************************************************
IntDisable:
	SWI     SWI_IRQ_DIS
	BX      lr
@ end of IntDisable

.end
@/*************************************************************************
@**                            End Of File
@**************************************************************************/


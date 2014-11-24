
;CodeVisionAVR C Compiler V2.05.3 Standard
;(C) Copyright 1998-2011 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type                : ATmega162
;Program type             : Application
;Clock frequency          : 8,000000 MHz
;Memory model             : Small
;Optimize for             : Speed
;(s)printf features       : int, width
;(s)scanf features        : int, width
;External RAM size        : 0
;Data Stack size          : 256 byte(s)
;Heap size                : 0 byte(s)
;Promote 'char' to 'int'  : Yes
;'char' is unsigned       : Yes
;8 bit enums              : Yes
;Global 'const' stored in FLASH     : No
;Enhanced function parameter passing: Yes
;Enhanced core instructions         : On
;Smart register allocation          : On
;Automatic register allocation      : On

	#pragma AVRPART ADMIN PART_NAME ATmega162
	#pragma AVRPART MEMORY PROG_FLASH 16384
	#pragma AVRPART MEMORY EEPROM 512
	#pragma AVRPART MEMORY INT_SRAM SIZE 1279
	#pragma AVRPART MEMORY INT_SRAM START_ADDR 0x100

	#define CALL_SUPPORTED 1

	.LISTMAC
	.EQU UDRE=0x5
	.EQU RXC=0x7
	.EQU USR=0xB
	.EQU UDR=0xC
	.EQU SPSR=0xE
	.EQU SPDR=0xF
	.EQU EERE=0x0
	.EQU EEWE=0x1
	.EQU EEMWE=0x2
	.EQU EECR=0x1C
	.EQU EEDR=0x1D
	.EQU EEARL=0x1E
	.EQU EEARH=0x1F
	.EQU WDTCR=0x21
	.EQU MCUCSR=0x34
	.EQU MCUCR=0x35
	.EQU EMCUCR=0x36
	.EQU GICR=0x3B
	.EQU SPL=0x3D
	.EQU SPH=0x3E
	.EQU SREG=0x3F

	.DEF R0X0=R0
	.DEF R0X1=R1
	.DEF R0X2=R2
	.DEF R0X3=R3
	.DEF R0X4=R4
	.DEF R0X5=R5
	.DEF R0X6=R6
	.DEF R0X7=R7
	.DEF R0X8=R8
	.DEF R0X9=R9
	.DEF R0XA=R10
	.DEF R0XB=R11
	.DEF R0XC=R12
	.DEF R0XD=R13
	.DEF R0XE=R14
	.DEF R0XF=R15
	.DEF R0X10=R16
	.DEF R0X11=R17
	.DEF R0X12=R18
	.DEF R0X13=R19
	.DEF R0X14=R20
	.DEF R0X15=R21
	.DEF R0X16=R22
	.DEF R0X17=R23
	.DEF R0X18=R24
	.DEF R0X19=R25
	.DEF R0X1A=R26
	.DEF R0X1B=R27
	.DEF R0X1C=R28
	.DEF R0X1D=R29
	.DEF R0X1E=R30
	.DEF R0X1F=R31

	.EQU __SRAM_START=0x0100
	.EQU __SRAM_END=0x04FF
	.EQU __DSTACK_SIZE=0x0100
	.EQU __HEAP_SIZE=0x0000
	.EQU __CLEAR_SRAM_SIZE=__SRAM_END-__SRAM_START+1

	.MACRO __CPD1N
	CPI  R30,LOW(@0)
	LDI  R26,HIGH(@0)
	CPC  R31,R26
	LDI  R26,BYTE3(@0)
	CPC  R22,R26
	LDI  R26,BYTE4(@0)
	CPC  R23,R26
	.ENDM

	.MACRO __CPD2N
	CPI  R26,LOW(@0)
	LDI  R30,HIGH(@0)
	CPC  R27,R30
	LDI  R30,BYTE3(@0)
	CPC  R24,R30
	LDI  R30,BYTE4(@0)
	CPC  R25,R30
	.ENDM

	.MACRO __CPWRR
	CP   R@0,R@2
	CPC  R@1,R@3
	.ENDM

	.MACRO __CPWRN
	CPI  R@0,LOW(@2)
	LDI  R30,HIGH(@2)
	CPC  R@1,R30
	.ENDM

	.MACRO __ADDB1MN
	SUBI R30,LOW(-@0-(@1))
	.ENDM

	.MACRO __ADDB2MN
	SUBI R26,LOW(-@0-(@1))
	.ENDM

	.MACRO __ADDW1MN
	SUBI R30,LOW(-@0-(@1))
	SBCI R31,HIGH(-@0-(@1))
	.ENDM

	.MACRO __ADDW2MN
	SUBI R26,LOW(-@0-(@1))
	SBCI R27,HIGH(-@0-(@1))
	.ENDM

	.MACRO __ADDW1FN
	SUBI R30,LOW(-2*@0-(@1))
	SBCI R31,HIGH(-2*@0-(@1))
	.ENDM

	.MACRO __ADDD1FN
	SUBI R30,LOW(-2*@0-(@1))
	SBCI R31,HIGH(-2*@0-(@1))
	SBCI R22,BYTE3(-2*@0-(@1))
	.ENDM

	.MACRO __ADDD1N
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	SBCI R22,BYTE3(-@0)
	SBCI R23,BYTE4(-@0)
	.ENDM

	.MACRO __ADDD2N
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	SBCI R24,BYTE3(-@0)
	SBCI R25,BYTE4(-@0)
	.ENDM

	.MACRO __SUBD1N
	SUBI R30,LOW(@0)
	SBCI R31,HIGH(@0)
	SBCI R22,BYTE3(@0)
	SBCI R23,BYTE4(@0)
	.ENDM

	.MACRO __SUBD2N
	SUBI R26,LOW(@0)
	SBCI R27,HIGH(@0)
	SBCI R24,BYTE3(@0)
	SBCI R25,BYTE4(@0)
	.ENDM

	.MACRO __ANDBMNN
	LDS  R30,@0+(@1)
	ANDI R30,LOW(@2)
	STS  @0+(@1),R30
	.ENDM

	.MACRO __ANDWMNN
	LDS  R30,@0+(@1)
	ANDI R30,LOW(@2)
	STS  @0+(@1),R30
	LDS  R30,@0+(@1)+1
	ANDI R30,HIGH(@2)
	STS  @0+(@1)+1,R30
	.ENDM

	.MACRO __ANDD1N
	ANDI R30,LOW(@0)
	ANDI R31,HIGH(@0)
	ANDI R22,BYTE3(@0)
	ANDI R23,BYTE4(@0)
	.ENDM

	.MACRO __ANDD2N
	ANDI R26,LOW(@0)
	ANDI R27,HIGH(@0)
	ANDI R24,BYTE3(@0)
	ANDI R25,BYTE4(@0)
	.ENDM

	.MACRO __ORBMNN
	LDS  R30,@0+(@1)
	ORI  R30,LOW(@2)
	STS  @0+(@1),R30
	.ENDM

	.MACRO __ORWMNN
	LDS  R30,@0+(@1)
	ORI  R30,LOW(@2)
	STS  @0+(@1),R30
	LDS  R30,@0+(@1)+1
	ORI  R30,HIGH(@2)
	STS  @0+(@1)+1,R30
	.ENDM

	.MACRO __ORD1N
	ORI  R30,LOW(@0)
	ORI  R31,HIGH(@0)
	ORI  R22,BYTE3(@0)
	ORI  R23,BYTE4(@0)
	.ENDM

	.MACRO __ORD2N
	ORI  R26,LOW(@0)
	ORI  R27,HIGH(@0)
	ORI  R24,BYTE3(@0)
	ORI  R25,BYTE4(@0)
	.ENDM

	.MACRO __DELAY_USB
	LDI  R24,LOW(@0)
__DELAY_USB_LOOP:
	DEC  R24
	BRNE __DELAY_USB_LOOP
	.ENDM

	.MACRO __DELAY_USW
	LDI  R24,LOW(@0)
	LDI  R25,HIGH(@0)
__DELAY_USW_LOOP:
	SBIW R24,1
	BRNE __DELAY_USW_LOOP
	.ENDM

	.MACRO __GETD1S
	LDD  R30,Y+@0
	LDD  R31,Y+@0+1
	LDD  R22,Y+@0+2
	LDD  R23,Y+@0+3
	.ENDM

	.MACRO __GETD2S
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	LDD  R24,Y+@0+2
	LDD  R25,Y+@0+3
	.ENDM

	.MACRO __PUTD1S
	STD  Y+@0,R30
	STD  Y+@0+1,R31
	STD  Y+@0+2,R22
	STD  Y+@0+3,R23
	.ENDM

	.MACRO __PUTD2S
	STD  Y+@0,R26
	STD  Y+@0+1,R27
	STD  Y+@0+2,R24
	STD  Y+@0+3,R25
	.ENDM

	.MACRO __PUTDZ2
	STD  Z+@0,R26
	STD  Z+@0+1,R27
	STD  Z+@0+2,R24
	STD  Z+@0+3,R25
	.ENDM

	.MACRO __CLRD1S
	STD  Y+@0,R30
	STD  Y+@0+1,R30
	STD  Y+@0+2,R30
	STD  Y+@0+3,R30
	.ENDM

	.MACRO __POINTB1MN
	LDI  R30,LOW(@0+(@1))
	.ENDM

	.MACRO __POINTW1MN
	LDI  R30,LOW(@0+(@1))
	LDI  R31,HIGH(@0+(@1))
	.ENDM

	.MACRO __POINTD1M
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __POINTW1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	.ENDM

	.MACRO __POINTD1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	LDI  R22,BYTE3(2*@0+(@1))
	LDI  R23,BYTE4(2*@0+(@1))
	.ENDM

	.MACRO __POINTB2MN
	LDI  R26,LOW(@0+(@1))
	.ENDM

	.MACRO __POINTW2MN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	.ENDM

	.MACRO __POINTW2FN
	LDI  R26,LOW(2*@0+(@1))
	LDI  R27,HIGH(2*@0+(@1))
	.ENDM

	.MACRO __POINTD2FN
	LDI  R26,LOW(2*@0+(@1))
	LDI  R27,HIGH(2*@0+(@1))
	LDI  R24,BYTE3(2*@0+(@1))
	LDI  R25,BYTE4(2*@0+(@1))
	.ENDM

	.MACRO __POINTBRM
	LDI  R@0,LOW(@1)
	.ENDM

	.MACRO __POINTWRM
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __POINTBRMN
	LDI  R@0,LOW(@1+(@2))
	.ENDM

	.MACRO __POINTWRMN
	LDI  R@0,LOW(@2+(@3))
	LDI  R@1,HIGH(@2+(@3))
	.ENDM

	.MACRO __POINTWRFN
	LDI  R@0,LOW(@2*2+(@3))
	LDI  R@1,HIGH(@2*2+(@3))
	.ENDM

	.MACRO __GETD1N
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __GETD2N
	LDI  R26,LOW(@0)
	LDI  R27,HIGH(@0)
	LDI  R24,BYTE3(@0)
	LDI  R25,BYTE4(@0)
	.ENDM

	.MACRO __GETB1MN
	LDS  R30,@0+(@1)
	.ENDM

	.MACRO __GETB1HMN
	LDS  R31,@0+(@1)
	.ENDM

	.MACRO __GETW1MN
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	.ENDM

	.MACRO __GETD1MN
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	LDS  R22,@0+(@1)+2
	LDS  R23,@0+(@1)+3
	.ENDM

	.MACRO __GETBRMN
	LDS  R@0,@1+(@2)
	.ENDM

	.MACRO __GETWRMN
	LDS  R@0,@2+(@3)
	LDS  R@1,@2+(@3)+1
	.ENDM

	.MACRO __GETWRZ
	LDD  R@0,Z+@2
	LDD  R@1,Z+@2+1
	.ENDM

	.MACRO __GETD2Z
	LDD  R26,Z+@0
	LDD  R27,Z+@0+1
	LDD  R24,Z+@0+2
	LDD  R25,Z+@0+3
	.ENDM

	.MACRO __GETB2MN
	LDS  R26,@0+(@1)
	.ENDM

	.MACRO __GETW2MN
	LDS  R26,@0+(@1)
	LDS  R27,@0+(@1)+1
	.ENDM

	.MACRO __GETD2MN
	LDS  R26,@0+(@1)
	LDS  R27,@0+(@1)+1
	LDS  R24,@0+(@1)+2
	LDS  R25,@0+(@1)+3
	.ENDM

	.MACRO __PUTB1MN
	STS  @0+(@1),R30
	.ENDM

	.MACRO __PUTW1MN
	STS  @0+(@1),R30
	STS  @0+(@1)+1,R31
	.ENDM

	.MACRO __PUTD1MN
	STS  @0+(@1),R30
	STS  @0+(@1)+1,R31
	STS  @0+(@1)+2,R22
	STS  @0+(@1)+3,R23
	.ENDM

	.MACRO __PUTB1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	CALL __EEPROMWRB
	.ENDM

	.MACRO __PUTW1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	CALL __EEPROMWRW
	.ENDM

	.MACRO __PUTD1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	CALL __EEPROMWRD
	.ENDM

	.MACRO __PUTBR0MN
	STS  @0+(@1),R0
	.ENDM

	.MACRO __PUTBMRN
	STS  @0+(@1),R@2
	.ENDM

	.MACRO __PUTWMRN
	STS  @0+(@1),R@2
	STS  @0+(@1)+1,R@3
	.ENDM

	.MACRO __PUTBZR
	STD  Z+@1,R@0
	.ENDM

	.MACRO __PUTWZR
	STD  Z+@2,R@0
	STD  Z+@2+1,R@1
	.ENDM

	.MACRO __GETW1R
	MOV  R30,R@0
	MOV  R31,R@1
	.ENDM

	.MACRO __GETW2R
	MOV  R26,R@0
	MOV  R27,R@1
	.ENDM

	.MACRO __GETWRN
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __PUTW1R
	MOV  R@0,R30
	MOV  R@1,R31
	.ENDM

	.MACRO __PUTW2R
	MOV  R@0,R26
	MOV  R@1,R27
	.ENDM

	.MACRO __ADDWRN
	SUBI R@0,LOW(-@2)
	SBCI R@1,HIGH(-@2)
	.ENDM

	.MACRO __ADDWRR
	ADD  R@0,R@2
	ADC  R@1,R@3
	.ENDM

	.MACRO __SUBWRN
	SUBI R@0,LOW(@2)
	SBCI R@1,HIGH(@2)
	.ENDM

	.MACRO __SUBWRR
	SUB  R@0,R@2
	SBC  R@1,R@3
	.ENDM

	.MACRO __ANDWRN
	ANDI R@0,LOW(@2)
	ANDI R@1,HIGH(@2)
	.ENDM

	.MACRO __ANDWRR
	AND  R@0,R@2
	AND  R@1,R@3
	.ENDM

	.MACRO __ORWRN
	ORI  R@0,LOW(@2)
	ORI  R@1,HIGH(@2)
	.ENDM

	.MACRO __ORWRR
	OR   R@0,R@2
	OR   R@1,R@3
	.ENDM

	.MACRO __EORWRR
	EOR  R@0,R@2
	EOR  R@1,R@3
	.ENDM

	.MACRO __GETWRS
	LDD  R@0,Y+@2
	LDD  R@1,Y+@2+1
	.ENDM

	.MACRO __PUTBSR
	STD  Y+@1,R@0
	.ENDM

	.MACRO __PUTWSR
	STD  Y+@2,R@0
	STD  Y+@2+1,R@1
	.ENDM

	.MACRO __MOVEWRR
	MOV  R@0,R@2
	MOV  R@1,R@3
	.ENDM

	.MACRO __INWR
	IN   R@0,@2
	IN   R@1,@2+1
	.ENDM

	.MACRO __OUTWR
	OUT  @2+1,R@1
	OUT  @2,R@0
	.ENDM

	.MACRO __CALL1MN
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	ICALL
	.ENDM

	.MACRO __CALL1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	CALL __GETW1PF
	ICALL
	.ENDM

	.MACRO __CALL2EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	CALL __EEPROMRDW
	ICALL
	.ENDM

	.MACRO __GETW1STACK
	IN   R26,SPL
	IN   R27,SPH
	ADIW R26,@0+1
	LD   R30,X+
	LD   R31,X
	.ENDM

	.MACRO __GETD1STACK
	IN   R26,SPL
	IN   R27,SPH
	ADIW R26,@0+1
	LD   R30,X+
	LD   R31,X+
	LD   R22,X
	.ENDM

	.MACRO __NBST
	BST  R@0,@1
	IN   R30,SREG
	LDI  R31,0x40
	EOR  R30,R31
	OUT  SREG,R30
	.ENDM


	.MACRO __PUTB1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RNS
	MOVW R26,R@0
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RNS
	MOVW R26,R@0
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RNS
	MOVW R26,R@0
	ADIW R26,@1
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	CALL __PUTDP1
	.ENDM


	.MACRO __GETB1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R30,Z
	.ENDM

	.MACRO __GETB1HSX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R31,Z
	.ENDM

	.MACRO __GETW1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R31,Z
	MOV  R30,R0
	.ENDM

	.MACRO __GETD1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R1,Z+
	LD   R22,Z+
	LD   R23,Z
	MOVW R30,R0
	.ENDM

	.MACRO __GETB2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R26,X
	.ENDM

	.MACRO __GETW2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	.ENDM

	.MACRO __GETD2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R1,X+
	LD   R24,X+
	LD   R25,X
	MOVW R26,R0
	.ENDM

	.MACRO __GETBRSX
	MOVW R30,R28
	SUBI R30,LOW(-@1)
	SBCI R31,HIGH(-@1)
	LD   R@0,Z
	.ENDM

	.MACRO __GETWRSX
	MOVW R30,R28
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	LD   R@0,Z+
	LD   R@1,Z
	.ENDM

	.MACRO __GETBRSX2
	MOVW R26,R28
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	LD   R@0,X
	.ENDM

	.MACRO __GETWRSX2
	MOVW R26,R28
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	LD   R@0,X+
	LD   R@1,X
	.ENDM

	.MACRO __LSLW8SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R31,Z
	CLR  R30
	.ENDM

	.MACRO __PUTB1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	.ENDM

	.MACRO __CLRW1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X,R30
	.ENDM

	.MACRO __CLRD1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X+,R30
	ST   X+,R30
	ST   X,R30
	.ENDM

	.MACRO __PUTB2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z,R26
	.ENDM

	.MACRO __PUTW2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z+,R26
	ST   Z,R27
	.ENDM

	.MACRO __PUTD2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z+,R26
	ST   Z+,R27
	ST   Z+,R24
	ST   Z,R25
	.ENDM

	.MACRO __PUTBSRX
	MOVW R30,R28
	SUBI R30,LOW(-@1)
	SBCI R31,HIGH(-@1)
	ST   Z,R@0
	.ENDM

	.MACRO __PUTWSRX
	MOVW R30,R28
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	ST   Z+,R@0
	ST   Z,R@1
	.ENDM

	.MACRO __PUTB1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	.ENDM

	.MACRO __MULBRR
	MULS R@0,R@1
	MOVW R30,R0
	.ENDM

	.MACRO __MULBRRU
	MUL  R@0,R@1
	MOVW R30,R0
	.ENDM

	.MACRO __MULBRR0
	MULS R@0,R@1
	.ENDM

	.MACRO __MULBRRU0
	MUL  R@0,R@1
	.ENDM

	.MACRO __MULBNWRU
	LDI  R26,@2
	MUL  R26,R@0
	MOVW R30,R0
	MUL  R26,R@1
	ADD  R31,R0
	.ENDM

;NAME DEFINITIONS FOR GLOBAL VARIABLES ALLOCATED TO REGISTERS
	.DEF __lcd_x=R5
	.DEF __lcd_y=R4
	.DEF __lcd_maxx=R7

	.CSEG
	.ORG 0x00

;START OF CODE MARKER
__START_OF_CODE:

;INTERRUPT VECTORS
	JMP  __RESET
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00
	JMP  0x00

_tbl10_G102:
	.DB  0x10,0x27,0xE8,0x3,0x64,0x0,0xA,0x0
	.DB  0x1,0x0
_tbl16_G102:
	.DB  0x0,0x10,0x0,0x1,0x10,0x0,0x1,0x0

_0x15:
	.DB  0x24,0x50,0x53,0x54,0x4D,0x53,0x41,0x56
	.DB  0x45,0x50,0x41,0x52,0xA,0x0,0x24,0x50
	.DB  0x53,0x54,0x4D,0x53,0x45,0x54,0x50,0x41
	.DB  0x52,0x2C,0x31,0x31,0x30,0x32,0x2C,0x38
	.DB  0x2A,0xA,0x0
_0x0:
	.DB  0x48,0x65,0x6C,0x6C,0x6F,0x5F,0x57,0x6F
	.DB  0x72,0x6C,0x64,0x21,0x21,0xA,0x31,0x32
	.DB  0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x2E
	.DB  0x0
_0x2000003:
	.DB  0x80,0xC0
_0x2020060:
	.DB  0x1
_0x2020000:
	.DB  0x2D,0x4E,0x41,0x4E,0x0,0x49,0x4E,0x46
	.DB  0x0

__GLOBAL_INI_TBL:
	.DW  0x02
	.DW  __base_y_G100
	.DW  _0x2000003*2

	.DW  0x01
	.DW  __seed_G101
	.DW  _0x2020060*2

_0xFFFFFFFF:
	.DW  0

__RESET:
	CLI
	CLR  R30
	OUT  EECR,R30

;INTERRUPT VECTORS ARE PLACED
;AT THE START OF FLASH
	LDI  R31,1
	OUT  GICR,R31
	OUT  GICR,R30
	OUT  MCUCR,R30
	OUT  EMCUCR,R30

;DISABLE WATCHDOG
	LDI  R31,0x18
	OUT  WDTCR,R31
	OUT  WDTCR,R30

;CLEAR R2-R14
	LDI  R24,(14-2)+1
	LDI  R26,2
	CLR  R27
__CLEAR_REG:
	ST   X+,R30
	DEC  R24
	BRNE __CLEAR_REG

;CLEAR SRAM
	LDI  R24,LOW(__CLEAR_SRAM_SIZE)
	LDI  R25,HIGH(__CLEAR_SRAM_SIZE)
	LDI  R26,LOW(__SRAM_START)
	LDI  R27,HIGH(__SRAM_START)
__CLEAR_SRAM:
	ST   X+,R30
	SBIW R24,1
	BRNE __CLEAR_SRAM

;GLOBAL VARIABLES INITIALIZATION
	LDI  R30,LOW(__GLOBAL_INI_TBL*2)
	LDI  R31,HIGH(__GLOBAL_INI_TBL*2)
__GLOBAL_INI_NEXT:
	LPM  R24,Z+
	LPM  R25,Z+
	SBIW R24,0
	BREQ __GLOBAL_INI_END
	LPM  R26,Z+
	LPM  R27,Z+
	LPM  R0,Z+
	LPM  R1,Z+
	MOVW R22,R30
	MOVW R30,R0
__GLOBAL_INI_LOOP:
	LPM  R0,Z+
	ST   X+,R0
	SBIW R24,1
	BRNE __GLOBAL_INI_LOOP
	MOVW R30,R22
	RJMP __GLOBAL_INI_NEXT
__GLOBAL_INI_END:

;HARDWARE STACK POINTER INITIALIZATION
	LDI  R30,LOW(__SRAM_END-__HEAP_SIZE)
	OUT  SPL,R30
	LDI  R30,HIGH(__SRAM_END-__HEAP_SIZE)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(__SRAM_START+__DSTACK_SIZE)
	LDI  R29,HIGH(__SRAM_START+__DSTACK_SIZE)

	JMP  _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x200

	.CSEG
;/*
;* tracker.c
;*
;* Created: 29.10.2012 12:47:26
;* Author: ivovk
;*/
;#define F_CPU 8000000
;
;#include <delay.h>
;//#include <avr/interrupt.h>
;#include <stdbool.h>
;// Alphanumeric LCD Module functions
;#asm
   .equ __lcd_port=0x1B ;PORTC
; 0000 000F #endasm
;#include <lcd.h>
;#include <mega162.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x20
	.SET power_ctrl_reg=mcucr
	#endif
;#include <stdint.h>
;#include <stdlib.h>
;#include <stdio.h>
;
;#define E  		 	   	  		      2 // pin number in selected port
;#define RS 		  	   	  		      0 // pin number in selected port
;#define D7 		 	   	  		      7 // pin number in selected port
;#define D6 		 	   	  		      6 // pin number in selected port
;#define D5 		 	   	  		      5 // pin number in selected port
;#define D4 	                          4 //
;
;char buffer[180];
;
;
;
;//===================================
;//Hardware initialization and reset
;//===================================
;
;/**
;Procedure of hardware initialization of atmega controller
;*/
;void initialization(){
; 0000 0028 void initialization(){

	.CSEG
_initialization:
; 0000 0029 #asm ("cli")
	cli
; 0000 002A // Crystal Oscillator division factor: 1
; 0000 002B #pragma optsize-
; 0000 002C CLKPR=0x80;
	LDI  R30,LOW(128)
	STS  97,R30
; 0000 002D CLKPR=0x00;
	LDI  R30,LOW(0)
	STS  97,R30
; 0000 002E #ifdef _OPTIMIZE_SIZE_
; 0000 002F #pragma optsize+
; 0000 0030 #endif
; 0000 0031 
; 0000 0032 // Input/Output Ports initialization
; 0000 0033 // Port A initialization
; 0000 0034 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 0035 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 0036 PORTA=0x00;
	OUT  0x1B,R30
; 0000 0037 DDRA = (1<<E)|(1<<RS)|(1<<D7)|(1<<D6)|(1<<D5)|(1<<D4);
	LDI  R30,LOW(245)
	OUT  0x1A,R30
; 0000 0038 
; 0000 0039 // Port B initialization
; 0000 003A // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 003B // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 003C PORTB=0x00;
	LDI  R30,LOW(0)
	OUT  0x18,R30
; 0000 003D DDRB=0x00;
	OUT  0x17,R30
; 0000 003E 
; 0000 003F // Port C initialization
; 0000 0040 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 0041 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 0042 PORTC=0x00;
	OUT  0x15,R30
; 0000 0043 DDRC=0x00;
	OUT  0x14,R30
; 0000 0044 
; 0000 0045 // Port D initialization
; 0000 0046 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 0047 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 0048 PORTD=0x00;
	OUT  0x12,R30
; 0000 0049 DDRD=0x00;
	OUT  0x11,R30
; 0000 004A 
; 0000 004B // Port E initialization
; 0000 004C // Func2=Out Func1=Out Func0=Out
; 0000 004D // State2=0 State1=0 State0=0
; 0000 004E PORTE=0x00;
	OUT  0x7,R30
; 0000 004F DDRE=0x07;
	LDI  R30,LOW(7)
	OUT  0x6,R30
; 0000 0050 
; 0000 0051 // Timer/Counter 0 initialization
; 0000 0052 // Clock source: System Clock
; 0000 0053 // Clock value: Timer 0 Stopped
; 0000 0054 // Mode: Normal top=0xFF
; 0000 0055 // OC0 output: Disconnected
; 0000 0056 TCCR0=0x00;
	LDI  R30,LOW(0)
	OUT  0x33,R30
; 0000 0057 TCNT0=0x00;
	OUT  0x32,R30
; 0000 0058 OCR0=0x00;
	OUT  0x31,R30
; 0000 0059 
; 0000 005A // Timer/Counter 1 initialization
; 0000 005B // Clock source: System Clock
; 0000 005C // Clock value: Timer1 Stopped
; 0000 005D // Mode: Normal top=0xFFFF
; 0000 005E // OC1A output: Discon.
; 0000 005F // OC1B output: Discon.
; 0000 0060 // Noise Canceler: Off
; 0000 0061 // Input Capture on Falling Edge
; 0000 0062 // Timer1 Overflow Interrupt: Off
; 0000 0063 // Input Capture Interrupt: Off
; 0000 0064 // Compare A Match Interrupt: Off
; 0000 0065 // Compare B Match Interrupt: Off
; 0000 0066 TCCR1A=0x00;
	OUT  0x2F,R30
; 0000 0067 TCCR1B=0x00;
	OUT  0x2E,R30
; 0000 0068 TCNT1H=0x00;
	OUT  0x2D,R30
; 0000 0069 TCNT1L=0x00;
	OUT  0x2C,R30
; 0000 006A ICR1H=0x00;
	OUT  0x25,R30
; 0000 006B ICR1L=0x00;
	OUT  0x24,R30
; 0000 006C OCR1AH=0x00;
	OUT  0x2B,R30
; 0000 006D OCR1AL=0x00;
	OUT  0x2A,R30
; 0000 006E OCR1BH=0x00;
	OUT  0x29,R30
; 0000 006F OCR1BL=0x00;
	OUT  0x28,R30
; 0000 0070 
; 0000 0071 // Timer/Counter 2 initialization
; 0000 0072 // Clock source: System Clock
; 0000 0073 // Clock value: Timer2 Stopped
; 0000 0074 // Mode: Normal top=0xFF
; 0000 0075 // OC2 output: Disconnected
; 0000 0076 ASSR=0x00;
	OUT  0x26,R30
; 0000 0077 TCCR2=0x00;
	OUT  0x27,R30
; 0000 0078 TCNT2=0x00;
	OUT  0x23,R30
; 0000 0079 OCR2=0x00;
	OUT  0x22,R30
; 0000 007A 
; 0000 007B // Timer/Counter 3 initialization
; 0000 007C // Clock value: Timer3 Stopped
; 0000 007D // Mode: Normal top=0xFFFF
; 0000 007E // OC3A output: Discon.
; 0000 007F // OC3B output: Discon.
; 0000 0080 // Noise Canceler: Off
; 0000 0081 // Input Capture on Falling Edge
; 0000 0082 // Timer3 Overflow Interrupt: Off
; 0000 0083 // Input Capture Interrupt: Off
; 0000 0084 // Compare A Match Interrupt: Off
; 0000 0085 // Compare B Match Interrupt: Off
; 0000 0086 TCCR3A=0x00;
	STS  139,R30
; 0000 0087 TCCR3B=0x00;
	STS  138,R30
; 0000 0088 TCNT3H=0x00;
	STS  137,R30
; 0000 0089 TCNT3L=0x00;
	STS  136,R30
; 0000 008A ICR3H=0x00;
	STS  129,R30
; 0000 008B ICR3L=0x00;
	STS  128,R30
; 0000 008C OCR3AH=0x00;
	STS  135,R30
; 0000 008D OCR3AL=0x00;
	STS  134,R30
; 0000 008E OCR3BH=0x00;
	STS  133,R30
; 0000 008F OCR3BL=0x00;
	STS  132,R30
; 0000 0090 
; 0000 0091 // External Interrupt(s) initialization
; 0000 0092 // INT0: Off
; 0000 0093 // INT1: Off
; 0000 0094 // INT2: Off
; 0000 0095 // Interrupt on any change on pins PCINT0-7: Off
; 0000 0096 // Interrupt on any change on pins PCINT8-15: Off
; 0000 0097 MCUCR=0x00;
	OUT  0x35,R30
; 0000 0098 EMCUCR=0x00;
	OUT  0x36,R30
; 0000 0099 
; 0000 009A // Timer(s)/Counter(s) Interrupt(s) initialization
; 0000 009B TIMSK=0x00;
	OUT  0x39,R30
; 0000 009C 
; 0000 009D ETIMSK=0x00;
	STS  125,R30
; 0000 009E 
; 0000 009F // USART0 initialization
; 0000 00A0 // Communication Parameters: 8 Data, 1 Stop, No Parity
; 0000 00A1 // USART0 Receiver: On
; 0000 00A2 // USART0 Transmitter: On
; 0000 00A3 // USART0 Mode: Asynchronous
; 0000 00A4 // USART0 Baud Rate: 115200
; 0000 00A5 UCSR0A=0x00;
	OUT  0xB,R30
; 0000 00A6 UCSR0B=0x18;
	LDI  R30,LOW(24)
	OUT  0xA,R30
; 0000 00A7 UCSR0C=0x86;
	LDI  R30,LOW(134)
	OUT  0x20,R30
; 0000 00A8 UBRR0H=0x00;
	LDI  R30,LOW(0)
	OUT  0x20,R30
; 0000 00A9 UBRR0L=0x03;
	LDI  R30,LOW(3)
	OUT  0x9,R30
; 0000 00AA 
; 0000 00AB // USART1 initialization
; 0000 00AC // Communication Parameters: 8 Data, 1 Stop, No Parity
; 0000 00AD // USART1 Receiver: On
; 0000 00AE // USART1 Transmitter: On
; 0000 00AF // USART1 Mode: Asynchronous
; 0000 00B0 // USART1 Baud Rate: 115200
; 0000 00B1 UCSR1A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2,R30
; 0000 00B2 UCSR1B=0x18;
	LDI  R30,LOW(24)
	OUT  0x1,R30
; 0000 00B3 UCSR1C=0x86;
	LDI  R30,LOW(134)
	OUT  0x3C,R30
; 0000 00B4 UBRR1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x3C,R30
; 0000 00B5 UBRR1L=0x03;
	LDI  R30,LOW(3)
	OUT  0x0,R30
; 0000 00B6 
; 0000 00B7 // Analog Comparator initialization
; 0000 00B8 // Analog Comparator: Off
; 0000 00B9 // Analog Comparator Input Capture by Timer/Counter 1: Off
; 0000 00BA ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
; 0000 00BB 
; 0000 00BC // SPI initialization
; 0000 00BD // SPI disabled
; 0000 00BE SPCR=0x00;
	LDI  R30,LOW(0)
	OUT  0xD,R30
; 0000 00BF // Alphanumeric LCD initialization
; 0000 00C0 // Connections are specified in the
; 0000 00C1 // Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
; 0000 00C2 // RS - PORTA Bit 0
; 0000 00C3 // RD - PORTA Bit 1
; 0000 00C4 // EN - PORTA Bit 2
; 0000 00C5 // D4 - PORTA Bit 4
; 0000 00C6 // D5 - PORTA Bit 5
; 0000 00C7 // D6 - PORTA Bit 6
; 0000 00C8 // D7 - PORTA Bit 7
; 0000 00C9 // Characters/line: 16
; 0000 00CA lcd_init(16);  //*/
	LDI  R26,LOW(16)
	CALL _lcd_init
; 0000 00CB #asm ("sei")
	sei
; 0000 00CC }
	RET
;
;
;#pragma used-
;
;#ifndef RXB8
;#define RXB8 1
;#endif
;
;#ifndef TXB8
;#define TXB8 0
;#endif
;
;#ifndef UPE
;#define UPE 2
;#endif
;
;#ifndef DOR
;#define DOR 3
;#endif
;
;#ifndef FE
;#define FE 4
;#endif
;
;#ifndef UDRE
;#define UDRE 5
;#endif
;
;#ifndef RXC
;#define RXC 7
;#endif
;
;#define FRAMING_ERROR (1<<FE)
;#define PARITY_ERROR (1<<UPE)
;#define DATA_OVERRUN (1<<DOR)
;#define DATA_REGISTER_EMPTY (1<<UDRE)
;#define RX_COMPLETE (1<<RXC)
;// Get a character from the USART1 Receiver
;#pragma used+
;
;char getchar1(void)
; 0000 00F6 {
_getchar1:
; 0000 00F7 char status,data;
; 0000 00F8 while (1)
	ST   -Y,R17
	ST   -Y,R16
;	status -> R17
;	data -> R16
_0x3:
; 0000 00F9       {
; 0000 00FA       while (((status=UCSR1A) & RX_COMPLETE)==0);
_0x6:
	IN   R30,0x2
	MOV  R17,R30
	ANDI R30,LOW(0x80)
	BREQ _0x6
; 0000 00FB       data=UDR1;
	IN   R16,3
; 0000 00FC //      lcd_clear();
; 0000 00FD //      if (FRAMING_ERROR) lcd_putsf("FRAMING_ERROR") ;
; 0000 00FE //      if (PARITY_ERROR) lcd_putsf("PARITY_ERROR") ;
; 0000 00FF //      if (DATA_OVERRUN) lcd_putsf("DATA_OVERRUN") ;
; 0000 0100 //
; 0000 0101 //      delay_ms(400);
; 0000 0102       if ((status )) //& (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
	CPI  R17,0
	BREQ _0x9
; 0000 0103          return data;
	MOV  R30,R16
	RJMP _0x20C0002
; 0000 0104       }
_0x9:
	RJMP _0x3
; 0000 0105 }
_0x20C0002:
	LD   R16,Y+
	LD   R17,Y+
	RET
;#pragma used-
;
;
;// Write a character to the USART1 Transmitter
;#pragma used+
;void putchar1(char c)
; 0000 010C {
; 0000 010D while ((UCSR1A & DATA_REGISTER_EMPTY)==0);
;	c -> Y+0
; 0000 010E UDR1=c;
; 0000 010F }
;
;/**
;Send reset signal to GPS chip
;*/
;void reset_gps(){
; 0000 0114 void reset_gps(){
_reset_gps:
; 0000 0115 	uint8_t UCSR1B_store = UCSR1B;//Save state of UCSR1B
; 0000 0116 	while( !( UCSR1A & (1<<UDRE1)) ){};//Wait for empty USART1 transmit buffer
	ST   -Y,R17
;	UCSR1B_store -> R17
	IN   R17,1
_0xD:
	SBIS 0x2,5
	RJMP _0xD
; 0000 0117 	UCSR1B &= ~(0x98);//Disable USART1 transmitter and receiver and interruption
	IN   R30,0x1
	ANDI R30,LOW(0x67)
	OUT  0x1,R30
; 0000 0118 
; 0000 0119 	DDRB = 0x9;// PB3 and PB0 set as out;
	LDI  R30,LOW(9)
	OUT  0x17,R30
; 0000 011A 	PORTB = 0x1;//Set HIGH level on /reset pin (PB0)
	LDI  R30,LOW(1)
	OUT  0x18,R30
; 0000 011B 	//TX1 pin is in Z state
; 0000 011C 	delay_ms(1);
	LDI  R26,LOW(1)
	LDI  R27,0
	CALL _delay_ms
; 0000 011D 
; 0000 011E 	PORTB = 0;//Set LOW level on /reset pin (PB0) for 15ms
	LDI  R30,LOW(0)
	OUT  0x18,R30
; 0000 011F 	delay_ms(15);
	LDI  R26,LOW(15)
	LDI  R27,0
	CALL _delay_ms
; 0000 0120 
; 0000 0121 	PORTB = 0x1;//Set HIGH level on /reset pin (PB0)
	LDI  R30,LOW(1)
	OUT  0x18,R30
; 0000 0122 	delay_ms(25);//Wait gps chip reset and set up to work mode
	LDI  R26,LOW(25)
	LDI  R27,0
	CALL _delay_ms
; 0000 0123 
; 0000 0124 	UCSR1B = UCSR1B_store;//Restore state of UCSR1B
	OUT  0x1,R17
; 0000 0125 }
	LD   R17,Y+
	RET
;
;/**
;Send emergency reset signal to GSM chip
;*/
;void e_reset_gsm(){
; 0000 012A void e_reset_gsm(){
; 0000 012B 	uint8_t UCSR0B_store = UCSR0B;//Save state of UCSR0B
; 0000 012C 	while( !( UCSR0A & (1<<UDRE0)) ){};//Wait for empty USART1 transmit buffer
;	UCSR0B_store -> R17
; 0000 012D 	UCSR0B &= ~(0x18);//Disable USART1 transmitter and receiver
; 0000 012E 
; 0000 012F 	DDRD = 0x16;// PD1 and PD2 and PD3 are out (TX, RTS, DTR)
; 0000 0130 	DDRE = 0x3;//PE0 and PE1 are out (Reset_gsm, IGN)
; 0000 0131 	delay_ms(1);
; 0000 0132 
; 0000 0133 	PORTE |= 0x2; //Set '1' on Reset_gsm pin
; 0000 0134 	delay_ms(15);
; 0000 0135 
; 0000 0136 	PORTE &= ~0x2;//Set Reset_gsm pin back to '0'
; 0000 0137 	delay_ms(500);//Wait 0.5s for GSM module set up
; 0000 0138 
; 0000 0139 	UCSR0B = UCSR0B_store;//Restore state of UCSR0B
; 0000 013A }
;
;/**
;Send ignition signal to GSM chip
;Block execution for 8 sec!
;*/
;void gsm_on(){
; 0000 0140 void gsm_on(){
; 0000 0141 	PORTE |= 0x1;//Set PE0 to '1' for 10ms
; 0000 0142 	delay_ms(100);
; 0000 0143 	PORTE &= ~0x1;//Set PE0 back to '0'
; 0000 0144 	delay_ms(80);
; 0000 0145 }
;
;//===================================
;//GSM chip processing
;//===================================
;
;/**
;Set all settings to necessary in GSM chip
;*/
;uint8_t set_settings_gsm(){
; 0000 014E uint8_t set_settings_gsm(){
; 0000 014F //	uint8_t i = 0;
; 0000 0150 //	ClearRxBuf();
; 0000 0151 //
; 0000 0152 //	sei();//Enable interruptions. Need for correct working of UART0(GSM) receiver
; 0000 0153 //
; 0000 0154 //	ClearRxBuf();
; 0000 0155 //	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY) ){
; 0000 0156 //		ClearRxBuf();
; 0000 0157 //		Send_AT_cmd("AT+IPR=115200\n\r", 1);//Setup fixed UART speed in gsm
; 0000 0158 //		i++;
; 0000 0159 //	}
; 0000 015A //
; 0000 015B //	ClearRxBuf();
; 0000 015C //	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 2) ){
; 0000 015D //		ClearRxBuf();
; 0000 015E //		Send_AT_cmd("ATE0\n\r", 1);//Disable echo mode
; 0000 015F //		i++;
; 0000 0160 //	}
; 0000 0161 //
; 0000 0162 //	ClearRxBuf();
; 0000 0163 //	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 3) ){
; 0000 0164 //		ClearRxBuf();
; 0000 0165 //		Send_AT_cmd("AT+CLCK=", 0);//Disable PIN code blocking. <AT+CLCK="SC",0,"PIN">
; 0000 0166 //		Send_AT_cmd(quote, 0);
; 0000 0167 //		Send_AT_cmd("SC", 0);
; 0000 0168 //		Send_AT_cmd(quote, 0);
; 0000 0169 //		Send_AT_cmd(",0,", 0);
; 0000 016A //		Send_AT_cmd(quote, 0);
; 0000 016B //		Send_AT_cmd(PIN, 0);
; 0000 016C //		Send_AT_cmd(quote, 0);
; 0000 016D //		Send_AT_cmd("\n\r", 1);
; 0000 016E //		i++;
; 0000 016F //	}
; 0000 0170 //
; 0000 0171 //	ClearRxBuf();
; 0000 0172 //	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 4) ){
; 0000 0173 //		ClearRxBuf();
; 0000 0174 //		Send_AT_cmd("AT\\Q3\n\r", 1);//Enable hardware data flow control
; 0000 0175 //		i++;
; 0000 0176 //	}
; 0000 0177 //
; 0000 0178 //	ClearRxBuf();
; 0000 0179 //	while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 5) ){
; 0000 017A //		ClearRxBuf();
; 0000 017B //		Send_AT_cmd("AT+CMGF=1\n\r", 1);//Set text mode for sms sending
; 0000 017C //		i++;
; 0000 017D //	}
; 0000 017E //
; 0000 017F //
; 0000 0180 //	//ClearRxBuf();
; 0000 0181 //	//while( (compare_st(RX_buf, "OK", 2) == false) || (i < TRY * 5) ){
; 0000 0182 //	//	ClearRxBuf();
; 0000 0183 //	//	Send_AT_cmd("AT^SSET=1\n\r", 1);//Enable URC <^SSIM READY>
; 0000 0184 //	//	i++;
; 0000 0185 //	//}
; 0000 0186 //
; 0000 0187 //	if(i == TRY * 5)
; 0000 0188 //		return -1;//All settings wasn't set successfully
; 0000 0189 //	else
; 0000 018A 		return 0;//All settings was set successfully
; 0000 018B }
;
;bool Gsm_Ready(){
; 0000 018D _Bool Gsm_Ready(){
; 0000 018E 	if(PIND & 0x28)//CTS(clear to send) DSR(always '1' by default)
; 0000 018F 		return true;
; 0000 0190 	else
; 0000 0191 		return false;
; 0000 0192 }
;
;
;//===================================
;//MAIN function
;//===================================
;
;void main(void){
; 0000 0199 void main(void){
_main:
; 0000 019A 	    int i, j;
; 0000 019B         char set[21] = "$PSTMSETPAR,1102,8*\n";
; 0000 019C         char save[14] = "$PSTMSAVEPAR\n";
; 0000 019D 
; 0000 019E 
; 0000 019F 	#asm ("cli")
	SBIW R28,35
	LDI  R24,35
	LDI  R26,LOW(0)
	LDI  R27,HIGH(0)
	LDI  R30,LOW(_0x15*2)
	LDI  R31,HIGH(_0x15*2)
	CALL __INITLOCB
;	i -> R16,R17
;	j -> R18,R19
;	set -> Y+14
;	save -> Y+0
	cli
; 0000 01A0 	initialization();
	RCALL _initialization
; 0000 01A1 
; 0000 01A2 	reset_gps();
	RCALL _reset_gps
; 0000 01A3 
; 0000 01A4 	//gsm_on();
; 0000 01A5 
; 0000 01A6 	//set_settings_gsm();
; 0000 01A7 
; 0000 01A8     lcd_clear();
	CALL _lcd_clear
; 0000 01A9 	lcd_putsf("Hello_World!!\n123456789.");
	__POINTW2FN _0x0,0
	CALL _lcd_putsf
; 0000 01AA 	delay_ms(2000);
	LDI  R26,LOW(2000)
	LDI  R27,HIGH(2000)
	CALL _delay_ms
; 0000 01AB     lcd_clear();
	CALL _lcd_clear
; 0000 01AC //	reset_gps();
; 0000 01AD     //get_gps_data();
; 0000 01AE //    i=0;
; 0000 01AF //    for (i = 0; i < 21; i++) {
; 0000 01B0 //        putchar1(set[i]);
; 0000 01B1 //        delay_ms(1);
; 0000 01B2 //    }
; 0000 01B3 //    for (i = 0; save[i] != '\n'; i++) {
; 0000 01B4 //        putchar1(save[i]);
; 0000 01B5 //        delay_ms(1);
; 0000 01B6 //    }
; 0000 01B7 	while(1){
_0x16:
; 0000 01B8         for (i = 0; i < 32; i++)
	__GETWRN 16,17,0
_0x1A:
	__CPWRN 16,17,32
	BRGE _0x1B
; 0000 01B9         {
; 0000 01BA             buffer[i] = getchar1();
	MOVW R30,R16
	SUBI R30,LOW(-_buffer)
	SBCI R31,HIGH(-_buffer)
	PUSH R31
	PUSH R30
	RCALL _getchar1
	POP  R26
	POP  R27
	ST   X,R30
; 0000 01BB             lcd_putchar(buffer[i]);
	LDI  R26,LOW(_buffer)
	LDI  R27,HIGH(_buffer)
	ADD  R26,R16
	ADC  R27,R17
	LD   R26,X
	CALL _lcd_putchar
; 0000 01BC 
; 0000 01BD         }
	__ADDWRN 16,17,1
	RJMP _0x1A
_0x1B:
; 0000 01BE         delay_ms(1000);
	LDI  R26,LOW(1000)
	LDI  R27,HIGH(1000)
	CALL _delay_ms
; 0000 01BF         lcd_clear();
	CALL _lcd_clear
; 0000 01C0 
; 0000 01C1 	}
	RJMP _0x16
; 0000 01C2 }
_0x1C:
	NOP
	RJMP _0x1C
    .equ __lcd_direction=__lcd_port-1
    .equ __lcd_pin=__lcd_port-2
    .equ __lcd_rs=0
    .equ __lcd_rd=1
    .equ __lcd_enable=2
    .equ __lcd_busy_flag=7

	.DSEG

	.CSEG
__lcd_delay_G100:
    ldi   r31,15
__lcd_delay0:
    dec   r31
    brne  __lcd_delay0
	RET
__lcd_ready:
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
    cbi   __lcd_port,__lcd_rs     ;RS=0
__lcd_busy:
	RCALL __lcd_delay_G100
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G100
    in    r26,__lcd_pin
    cbi   __lcd_port,__lcd_enable ;EN=0
	RCALL __lcd_delay_G100
    sbi   __lcd_port,__lcd_enable ;EN=1
	RCALL __lcd_delay_G100
    cbi   __lcd_port,__lcd_enable ;EN=0
    sbrc  r26,__lcd_busy_flag
    rjmp  __lcd_busy
	RET
__lcd_write_nibble_G100:
    andi  r26,0xf0
    or    r26,r27
    out   __lcd_port,r26          ;write
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G100
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G100
	RET
__lcd_write_data:
	ST   -Y,R26
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf0 | (1<<__lcd_rs) | (1<<__lcd_rd) | (1<<__lcd_enable) ;set as output
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	RCALL __lcd_write_nibble_G100
    ld    r26,y
    swap  r26
	RCALL __lcd_write_nibble_G100
    sbi   __lcd_port,__lcd_rd     ;RD=1
	JMP  _0x20C0001
__lcd_read_nibble_G100:
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G100
    in    r30,__lcd_pin           ;read
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G100
    andi  r30,0xf0
	RET
_lcd_read_byte0_G100:
	CALL __lcd_delay_G100
	RCALL __lcd_read_nibble_G100
    mov   r26,r30
	RCALL __lcd_read_nibble_G100
    cbi   __lcd_port,__lcd_rd     ;RD=0
    swap  r30
    or    r30,r26
	RET
_lcd_gotoxy:
	ST   -Y,R26
	CALL __lcd_ready
	LD   R30,Y
	LDI  R31,0
	SUBI R30,LOW(-__base_y_G100)
	SBCI R31,HIGH(-__base_y_G100)
	LD   R30,Z
	LDD  R26,Y+1
	ADD  R26,R30
	CALL __lcd_write_data
	LDD  R5,Y+1
	LDD  R4,Y+0
	ADIW R28,2
	RET
_lcd_clear:
	CALL __lcd_ready
	LDI  R26,LOW(2)
	CALL __lcd_write_data
	CALL __lcd_ready
	LDI  R26,LOW(12)
	CALL __lcd_write_data
	CALL __lcd_ready
	LDI  R26,LOW(1)
	CALL __lcd_write_data
	LDI  R30,LOW(0)
	MOV  R4,R30
	MOV  R5,R30
	RET
_lcd_putchar:
	ST   -Y,R26
    push r30
    push r31
    ld   r26,y
    set
    cpi  r26,10
    breq __lcd_putchar1
    clt
	CP   R5,R7
	BRLO _0x2000004
	__lcd_putchar1:
	INC  R4
	LDI  R30,LOW(0)
	ST   -Y,R30
	MOV  R26,R4
	RCALL _lcd_gotoxy
	brts __lcd_putchar0
_0x2000004:
	INC  R5
    rcall __lcd_ready
    sbi  __lcd_port,__lcd_rs ;RS=1
	LD   R26,Y
	CALL __lcd_write_data
__lcd_putchar0:
    pop  r31
    pop  r30
	JMP  _0x20C0001
_lcd_putsf:
	ST   -Y,R27
	ST   -Y,R26
	ST   -Y,R17
_0x2000008:
	LDD  R30,Y+1
	LDD  R31,Y+1+1
	ADIW R30,1
	STD  Y+1,R30
	STD  Y+1+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R17,R30
	CPI  R30,0
	BREQ _0x200000A
	MOV  R26,R17
	RCALL _lcd_putchar
	RJMP _0x2000008
_0x200000A:
	LDD  R17,Y+0
	ADIW R28,3
	RET
__long_delay_G100:
    clr   r26
    clr   r27
__long_delay0:
    sbiw  r26,1         ;2 cycles
    brne  __long_delay0 ;2 cycles
	RET
__lcd_init_write_G100:
	ST   -Y,R26
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf7                ;set as output
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	CALL __lcd_write_nibble_G100
    sbi   __lcd_port,__lcd_rd     ;RD=1
	RJMP _0x20C0001
_lcd_init:
	ST   -Y,R26
    cbi   __lcd_port,__lcd_enable ;EN=0
    cbi   __lcd_port,__lcd_rs     ;RS=0
	LDD  R7,Y+0
	LD   R30,Y
	SUBI R30,-LOW(128)
	__PUTB1MN __base_y_G100,2
	LD   R30,Y
	SUBI R30,-LOW(192)
	__PUTB1MN __base_y_G100,3
	RCALL __long_delay_G100
	LDI  R26,LOW(48)
	RCALL __lcd_init_write_G100
	RCALL __long_delay_G100
	LDI  R26,LOW(48)
	RCALL __lcd_init_write_G100
	RCALL __long_delay_G100
	LDI  R26,LOW(48)
	RCALL __lcd_init_write_G100
	RCALL __long_delay_G100
	LDI  R26,LOW(32)
	RCALL __lcd_init_write_G100
	RCALL __long_delay_G100
	LDI  R26,LOW(40)
	CALL __lcd_write_data
	RCALL __long_delay_G100
	LDI  R26,LOW(4)
	CALL __lcd_write_data
	RCALL __long_delay_G100
	LDI  R26,LOW(133)
	CALL __lcd_write_data
	RCALL __long_delay_G100
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	CALL _lcd_read_byte0_G100
	CPI  R30,LOW(0x5)
	BREQ _0x200000B
	LDI  R30,LOW(0)
	RJMP _0x20C0001
_0x200000B:
	CALL __lcd_ready
	LDI  R26,LOW(6)
	CALL __lcd_write_data
	CALL _lcd_clear
	LDI  R30,LOW(1)
_0x20C0001:
	ADIW R28,1
	RET

	.CSEG

	.DSEG

	.CSEG
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x20
	.SET power_ctrl_reg=mcucr
	#endif

	.CSEG

	.CSEG

	.CSEG

	.CSEG

	.DSEG
_buffer:
	.BYTE 0xB4
__base_y_G100:
	.BYTE 0x4
__seed_G101:
	.BYTE 0x4

	.CSEG

	.CSEG
_delay_ms:
	adiw r26,0
	breq __delay_ms1
__delay_ms0:
	__DELAY_USW 0x7D0
	wdr
	sbiw r26,1
	brne __delay_ms0
__delay_ms1:
	ret

__INITLOCB:
__INITLOCW:
	ADD  R26,R28
	ADC  R27,R29
__INITLOC0:
	LPM  R0,Z+
	ST   X+,R0
	DEC  R24
	BRNE __INITLOC0
	RET

;END OF CODE MARKER
__END_OF_CODE:

;asp stuff, some other assembly
;author ryan smith
;
;targets a tm4c123gh6pm/ek-tm4c123gxl
;
;First must setup PSP first.
; void setpsp(uint32_t * p)
;then set ASP bit =1
;
;
;

.thumb
.text


;;;;;;;;;;;;;;;;;;;;;;;;;
.def setASPBit

setASPBit:
		MRS	R0, CONTROL	;
		ORR	R0, R0, #2	;
		MSR	CONTROL, R0	;
		ISB				;
		BX	LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
.def setpsp

setpsp:
		MSR PSP, R0		;	set the psp to what is loaded in R0 maybe?

;;;;;;;;;;;;;;;;;;;;;;;;;
.def getR0

getR0:					;
		BX 	LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
.def getR1
getR1:
		MOV R0,R1
		BX LR

;;;;;;;;;;;;;;;;;;;;;;;;;
.def getR2
getR2:
		MOV R0,R2
		BX LR

;;;;;;;;;;;;;;;;;;;;;;;;;
.def getR3
getR3:
		MOV R0,R3
		BX LR


;;;;;;;;;;;;;;;;;;;;;;;;;
.def getR7

getR7:					;
		MOV	R0, R7		;
		BX	LR			;

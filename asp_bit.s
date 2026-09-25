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

	.def setASPBit
	.def setpsp
	.def getpsp
	.def getmsp




.thumb
.text


;;;;;;;;;;;;;;;;;;;;;;;;;
;	set the asp bit and then send the isb then escape.
setASPBit:
		MRS	R0, CONTROL	;
		ORR	R0, R0, #2	;
		MSR	CONTROL, R0	;
		ISB				;
		BX	LR			;


;;;;;;;;;;;;;;;;;;;;;;;;;
;	set the psp to what is loaded in R0 maybe?
setpsp:
		MSR PSP, R0		;
		BX LR


;;;;;;;;;;;;;;;;;;;;;;;;;
;	Grab the psp address
getpsp:					;
		MRS R0, PSP		;
		BX 	LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	Grab the msp address

getmsp:					;
		MRS R0, MSP		;
		BX LR			;


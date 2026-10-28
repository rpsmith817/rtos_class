;asp stuff, some other assembly
;author ryan smith
;
;targets a tm4c123gh6pm/ek-tm4c123gxl
;
;

	.def setASPBit
	.def setTmpl
	.def setpsp
	.def getpsp
	.def getmsp
	.def memBarrier
	.def asm_pop
	.def asm_pull
	.def asm_svc_1
	.def asm_svc_2
	.def asm_svc_3
	.def asm_svc_4
	.def asm_svc_5
	.def asm_svc_6
	.def asm_svc_7
	.def asm_svc_8


.thumb					;per instructor thumb instruction set
.text


;;;;;;;;;;;;;;;;;;;;;;;;;
;	set the asp bit and then send the isb then escape.
setASPBit:				;
		MRS	R0, CONTROL	;
		ORR	R0, R0, #2	;
		MSR	CONTROL, R0	;
		ISB				;
		BX	LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	go to unprivileged mode per losh
setTmpl:				;
		MRS R0, CONTROL	;
		ORR R0,R0,#1	;
		MSR CONTROL, R0 ;
		ISB				;
		BX LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	set the psp to what is loaded in R0
setpsp:					;
		MSR PSP, R0		;
		BX LR			;

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

;;;;;;;;;;;;;;;;;;;;;;;;;
;	instruction barrier and data sync barriers.
memBarrier:				;
		DSB				;
		ISB				;
		BX LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	pop stack values into R0 (prototype discards return)
asm_pop:				;
		POP {R0}		;
		BX LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	pull r4:r11 onto the sp. kinda bad to do when we are tryna save some registers, except that the spot we are using is in the msp so it is fine.
;   clues found from https://www.wadixtech.com/blog/arm-calling-convention-on-cortex-m-how-arguments-registers-and-the-stack-really-work
asm_pull:
		MRS R0, MSP		;
		STMDB R0!,{R4-R11};
		BX LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	call svc with imm 1
asm_svc_1:				;
		SVC #1			;
		BX LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	call svc with imm 2
asm_svc_2:				;
		SVC #2			;
		BX LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	call svc with imm 3
asm_svc_3:				;
		SVC #3			;
		BX LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	call svc with imm 4
asm_svc_4:				;
		SVC #4			;
		BX LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	call svc with imm 5
asm_svc_5:				;
		SVC #5			;
		BX LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	call svc with imm 6
asm_svc_6:				;
		SVC #6			;
		BX LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	call svc with imm 7
asm_svc_7:				;
		SVC #7			;
		BX LR			;

;;;;;;;;;;;;;;;;;;;;;;;;;
;	call svc with imm 8
asm_svc_8:				;
		SVC #8			;
		BX LR			;


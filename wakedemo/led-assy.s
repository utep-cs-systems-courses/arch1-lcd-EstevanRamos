	.arch msp430g2553
	.p2align 1,0
	.text
	
	.global alternate_leds
	.extern P1OUT


alternate_leds:
	mov #125, r13
	cmp r12, r13
	jl red
	bis.b #1, &P1OUT
	and.b #~64, &P1OUT
	jmp exit
red:
	mov #250 , r13
	cmp r12 , r13
	jl exit
	bis.b #64, &P1OUT
	and.b #~1, &P1OUT
	jmp exit
exit:
	pop r0

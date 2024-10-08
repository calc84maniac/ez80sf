; IEEE single precision minimum
; aubc = fmin(aubc, euhl)
__fpmin: ; CHECK: sameignzerosign(bitcast(float, pair8_24_t, { out.BC, out.A }), fminf(quiet(bitcast(float, pair8_24_t, { in.BC, in.A })), quiet(bitcast(float, pair8_24_t, { in.HL, in.E })))) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	call	__fpcmpo
	ret	p
	jq	nc, __fpmax.unordered
.return:
	push	hl
	pop	bc
	ld	a, e
	ret

; IEEE single precision maximum
; aubc = fmax(aubc, euhl)
__fpmax: ; CHECK: sameignzerosign(bitcast(float, pair8_24_t, { out.BC, out.A }), fmaxf(quiet(bitcast(float, pair8_24_t, { in.BC, in.A })), quiet(bitcast(float, pair8_24_t, { in.HL, in.E })))) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	call	__fpcmpo
	ret	c
	jq	p, __fpmin.return
.unordered:
	; Compare the low 31 bits (ignoring sign) and return the smaller one, which is non-NaN if either one is
	sbc	hl, bc
	ccf
	sbc	a, e
	add	hl, bc
	ccf
	adc	a, e
	; Carry into bit 31 is equivalent to carry XOR overflow
	jq	nc, .nocarry
	ret	po
;	jq	.return
	virtual
		jr	nc, $
		load .jr_nc : byte from $$
	end virtual
	db	.jr_nc
.nocarry:
	ret	pe
.return:
	jq	__fpmin.return
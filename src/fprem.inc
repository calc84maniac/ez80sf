; IEEE single precision remainder
; aubc = fmod(aubc, euhl)
__fprem: ; CHECK: same(bitcast(float, pair8_24_t, { out.BC, out.A }), fmodf(bitcast(float, pair8_24_t, { in.BC, in.A }), bitcast(float, pair8_24_t, { in.HL, in.E }))) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	push	de
	ld	d, a
	set	7, a	; aubc = -abs(AUBC)
	set	7, e	; euhl = -abs(EUHL)
	call	__fpcmpo	; abs(AUBC) <=> abs(EUHL)
	jq	m, .notge	; !(abs(AUBC) >= abs(EUHL))
	push	hl, bc
	call	__fpupop1
	ex	(sp), hl
	ld	a, e
	ld	e, d
	call	__fpupop2
	ld	b, a
	cpl
	inc	e
	add	a, e	; Always sets carry, unless dividend non-finite
	ld	e, d
	ld	d, b
	pop	bc
	push	de
	ex	de, hl
	sbc	hl, hl
	add	hl, bc
	jq	nc, .nan	; Return if dividend non-finite or divisor zero
	sbc	hl, hl
	or	a, a
	sbc	hl, bc
	inc	hl
	ex	de, hl
	call	m, __iremu	; If divisor subnormal, modulo the mantissas
	ld	b, a
	inc	b
.rem.compare:
	add	hl, de
	jq	c, .rem.norestore
	sbc	hl, de
.rem.norestore:
	dec	b
	jq	z, .rem.finish
.rem.loop:
	add	hl, hl
	jq	nc, .rem.compare
	add	hl, de
	djnz	.rem.loop
.rem.finish:
	pop	bc
	rlc	c
	ld	de, 0800000h
	xor	a, a
	call	__fppack
	ex	(sp), hl
	pop	bc, de
	ret

.nan:
	pop	hl, hl
.notge:
	ld	a, d
	pop	de
	ret	c	; abs(AUBC) < abs(EUHL)
	ld	bc, 0C00000h
	ld	a, 0FFh
	ret


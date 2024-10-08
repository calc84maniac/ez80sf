; IEEE single precision division
; aubc = aubc / euhl
__fpdiv: ; CHECK: same(bitcast(float, pair8_24_t, { out.BC, out.A }), bitcast(float, pair8_24_t, { in.BC, in.A }) / bitcast(float, pair8_24_t, { in.HL, in.E })) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	push	de, hl
	xor	a, e
	push	af
	xor	a, e
	push	bc
	call	__fpupop1
	ex	(sp), hl
	ld	d, e
	ld	e, a
	call	__fpupop2
	inc	e
	jq	z, .nonfinite.1
	ld	a, d
	inc	a
	jq	z, .nonfinite.2
	add	a, 080h
	dec	d
	jq	nz, .exponent.adjust
	ex	(sp), hl
	add	hl, bc
	jq	c, .normalize.divisor.done
	sbc	hl, bc
	jq	z, .divisor.zero
.normalize.divisor.loop:
	dec	a
	add	hl, hl
	add	hl, bc
	jq	nc, .normalize.divisor.loop
.normalize.divisor.done:
	add	hl, bc
	ex	(sp), hl
.exponent.adjust:
	ex	de, hl	; ude=UHL, uhl=UDE
	ld	h, b
	rl	b
	ld	c, a
	sbc	hl, bc
	ld	b, h
	ld	c, l
	pop	hl
	add	hl, de	; uhl=UHL+UDE
	ex	de, hl	; ude=UHL+UDE, uhl=UHL
	xor	a, a
	sbc	hl, de	; uhl=-UDE
	ex	de, hl	; ude=-UDE, uhl=UHL+UDE
	add	hl, de	; uhl=UHL
.normalize.dividend.loop:
	add	hl, de
	jq	c, .normalize.dividend.done
	sbc	hl, de
	jq	z, .dividend.zero
	dec	bc
	add	hl, hl
	jq	nc, .normalize.dividend.loop
	add	hl, de
.normalize.dividend.done:
	cp	a, b
	jq	nc, .return.overflow
	dec	bc
	dec	bc
	ld	a, c
	inc	b
	ld	bc, 0800000h or (23 shl 8)
	jq	z, .divide.entry.normal
	adc	a, b
	ld	b, a
	ld	a, c
	jq	nc, .return.underflow
	push	iy
	ld	iy, 0
	jq	nz, .divide.entry.subnormal
	dec	hl
	add	hl, de
	jq	.subsubnormal

.divisor.zero:
	pop	de
	sbc	hl, de
	jq	nz, .return.nonfinite
.return.nan:
	inc	b
.return.overflow:
	ld	c, b
.return.nonfinite:
	pop	af, hl, de
	or	a, 07Fh
	ret

.nonfinite.1:
	inc	d
	pop	de
	jq	z, .return.nan
	push	hl
	pop	bc
	jq	.return.nonfinite

.nonfinite.2:
	pop	hl
	dec	hl
	add	hl, bc
	jq	c, .return.nan
.return.underflow:
	sbc	hl, hl
.dividend.zero:
	pop	de
	jq	.return

.divide.entry.normal:
	push	iy
	ld	iyl, b
.divide.loop:
	add	iy, iy
	add	hl, hl
	jq	c, .divide.overflow
	add	hl, de
	jq	c, .divide.setbit
	sbc	hl, de
	djnz	.divide.loop
	add	hl, hl
	jq	.divide.finish
.divide.overflow:
	add	hl, de
.divide.setbit:
.divide.entry.subnormal:
	inc	iy
	djnz	.divide.loop
	add	hl, hl
	inc	hl
.divide.finish:
	jq	c, .round
	dec	de
	add	hl, de
.round:
	ccf
.subsubnormal:
	lea	de, iy
	pop	iy
	sbc	hl, hl
	inc	hl
	add	hl, de
	pop	de
	ld	e, a
	adc	a, 1
	srl	e
	jq	nc, .return
	add	hl, bc
.return:
	sla	d
	rra
	ex	(sp), hl
	pop	bc, de
	ret

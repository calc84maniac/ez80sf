; IEEE single precision floor
; aubc = floor(aubc)
__fpfloor: ; CHECK: same(bitcast(float, pair8_24_t, { out.BC, out.A }), floorf(bitcast(float, pair8_24_t, { in.BC, in.A }))) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	cp	a, 080h
	jq	c, __fptrunc
.roundup:
	push	bc
	ex	(sp), hl
	adc	hl, hl
	ld	h, a
	rla
	jq	z, .mantissa.zero
	sub	a, 07Fh
	jq	c, .return.one
	sub	a, 24
	jq	nc, .return
	push	bc
	cpl
	ld	c, a
	ld	a, h
	sbc	hl, hl
	call	__ishl
	ex	(sp), hl
	pop	bc
	dec	hl
	call	__iand
	or	a, a
	sbc	hl, bc
	sbc	a, -1
	push	hl
	pop	bc
	ld	h, a
.return:
	ld	a, h
	pop	hl
	ret

.mantissa.zero:
	dec	a
	cp	a, 07Fh - 1
	jq	nc, .return
.return.one:
	ld	bc, 0800000h
	ld	a, h
	or	a, 07Fh shr 1
	pop	hl
	ret

; IEEE single precision ceiling
; aubc = ceil(aubc)
__fpceil: ; CHECK: same(bitcast(float, pair8_24_t, { out.BC, out.A }), ceilf(bitcast(float, pair8_24_t, { in.BC, in.A }))) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	rlca
	rrca
	jq	nc, __fpfloor.roundup

; IEEE single precision truncation
; aubc = trunc(aubc)
__fptrunc: ; CHECK: same(bitcast(float, pair8_24_t, { out.BC, out.A }), truncf(bitcast(float, pair8_24_t, { in.BC, in.A }))) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	push	bc
	ex	(sp), hl
	add	hl, hl
	ld	h, a
	rla
	sub	a, 07Fh
	jq	c, .return.zero
	sub	a, 24
	jq	nc, .return
	ld	l, c
	cpl
	ld	c, a
	ld	a, l
	push	hl
	sbc	hl, hl
	call	__ishl
	ld	c, a
	call	__iand
	ex	(sp), hl
	pop	bc
.return:
	ld	a, h
	pop	hl
	ret

.return.zero:
	ld	bc, 0
	ld	a, h
	and	a, 080h
	pop	hl
	ret

; IEEE single precision to 32-bit integers
; aubc = long(aubc)
__fptol: ; PREREQ: bitcast(float, pair8_24_t, { in.BC, in.A })-INT32_MIN > -1.0f && bitcast(float, pair8_24_t, { in.BC, in.A }) < (INT32_MAX/2+1)*2.0f CHECK: bitcast(int32_t, pair8_24_t, { out.BC, out.A }) == (int32_t)bitcast(float, pair8_24_t, { in.BC, in.A }) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
__fptoul: ; PREREQ: bitcast(float, pair8_24_t, { in.BC, in.A }) > -1.0f && bitcast(float, pair8_24_t, { in.BC, in.A }) < (UINT32_MAX/2+1)*2.0f CHECK: bitcast(uint32_t, pair8_24_t, { out.BC, out.A }) == (uint32_t)bitcast(float, pair8_24_t, { in.BC, in.A }) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	push	de, bc
	ex	(sp), hl
	ld	d, a
	ld	e, a
	call	__fpupop1
	ld	a, e
	sub	a, 07Fh
	cp	a, 23 + 32
	jq	nc, .return.zero
	sub	a, 23 + 1
	jq	nc, .left
	cpl
	ld	c, a
	call	__ishru
	xor	a, a
	jq	.finish
.left:
	inc	a
	ld	b, a
	xor	a, a
.loop:
	add	hl, hl
	rla
	djnz	.loop
.finish:
	ld	e, a
	bit	7, d
	call	nz, __lneg
	ld	a, e
.return:
	ex	(sp), hl
	pop	bc, de
	ret

.return.zero:
	xor	a, a
	sbc	hl, hl
	jq	.return

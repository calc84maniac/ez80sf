; IEEE single precision round
; aubc = round(aubc)
__fpround: ; CHECK: same(bitcast(float, pair8_24_t, { out.BC, out.A }), roundf(bitcast(float, pair8_24_t, { in.BC, in.A }))) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	push	bc
	ex	(sp), hl
	add	hl, hl
	ld	h, a
	rla
	sub	a, 07Fh - 1
	jq	c, .return.zero
	sub	a, 24
	jq	nc, .return
	push	bc
	cpl
	ld	c, a
	ld	a, h
	sbc	hl, hl
	call	__ishl
	ex	de, hl
	ex	(sp), hl
	or	a, a
	sbc	hl, de
	ex	(sp), hl
	ex	de, hl
	jq	pe, .overflow
	jq	c, .no_overflow
	inc	a
;	jq	.overflow
	virtual
		jr	c, $
		load .jr_c : byte from $$
	end virtual
	db	.jr_c
.no_overflow:
	add	hl, hl
.overflow:
	pop	bc
	call	__iand
	push	hl
	pop	bc
	ld	h, a
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

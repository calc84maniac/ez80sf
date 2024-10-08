; IEEE single precision from 32-bit integers
; aubc = float(aubc)
__ultofp: ; CHECK: same(bitcast(float, pair8_24_t, { out.BC, out.A }), (float)bitcast(uint32_t, pair8_24_t, { in.BC, in.A })) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	push	af
	xor	a, a
;	jq	__ltofp.enter
	virtual
		jr	c, $
		load .jr_c : byte from $$
	end virtual
	db	.jr_c

__ltofp: ; CHECK: same(bitcast(float, pair8_24_t, { out.BC, out.A }), (float)bitcast(int32_t, pair8_24_t, { in.BC, in.A })) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	push	af
.enter:
	rlca
	inc	sp
	push	bc
	inc	sp
	ex	(sp), hl
	ld	b, c
	ld	c, a
	ld	a, b
	push	de
	jq	nc, .positive
	add	hl, de
	ex	de, hl
	neg
	sbc	hl, de
.positive:
	ld	de, 0800000h
	ld	b, 07Fh + 31
	call	__fppack
	pop	de
	ex	(sp), hl
	pop	bc
	inc	sp
	ret

; IEEE single precision from 64-bit integers
; euhl = float(bcudeuhl)
__lltofp: ; CHECK: same(bitcast(float, pair8_24_t, { out.HL, out.E }), (float)bitcast(int64_t, tuple16_24_24_t, { in.HL, in.DE, in.BCS })) && out.A == in.A && out.BC == in.BC && out.IX == in.IX && out.IY == in.IY
	push	af, bc
	ld	a, b
	or	a, a
	call	m, __llneg
	jq	__ulltofp.enter

__ulltofp: ; CHECK: same(bitcast(float, pair8_24_t, { out.HL, out.E }), (float)bitcast(uint64_t, tuple16_24_24_t, { in.HL, in.DE, in.BCS })) && out.A == in.A && out.BC == in.BC && out.IX == in.IX && out.IY == in.IY
	push	af, bc
	xor	a, a
.enter:
	inc	b
	djnz	.upper
	inc	c
	dec	c
	jq	z, .lower
.upper:
	push	bc
	push	de
	push	hl
	ld	c, a
	ld	a, l
	or	a, h
	inc	sp
	inc	sp
	pop	hl
	pop	de
	inc	sp
	or	a, l
	ld	l, a
	ld	b, 07Fh + 63
;	jq	.pack
	virtual
		jp	c, $
		load .jp_c : byte from $$
	end virtual
	db	.jp_c
.lower:
	ld	c, a
	ld	b, 07Fh + 47
.pack:
	ex	de, hl
	push	de
	ex	(sp), ix
	ld	de, 0800000h
	call	__fppack2
	ld	e, a
	pop	ix, bc, af
	ret

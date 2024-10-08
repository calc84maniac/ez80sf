; IEEE single precision subtraction
; aubc = aubc - euhl
__fpsub: ; CHECK: same(bitcast(float, pair8_24_t, { out.BC, out.A }), bitcast(float, pair8_24_t, { in.BC, in.A }) - bitcast(float, pair8_24_t, { in.HL, in.E })) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	push	de
	rl	e
	ccf
	rr	e
;	jq	__fpadd.enter
	virtual
		ld	d, 0
		load .ld_d : byte from $$
	end virtual
	db	.ld_d

; IEEE single precision addition
; aubc = aubc + euhl
__fpadd: ; CHECK: same(bitcast(float, pair8_24_t, { out.BC, out.A }), bitcast(float, pair8_24_t, { in.BC, in.A }) + bitcast(float, pair8_24_t, { in.HL, in.E })) && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	push	de
.enter:
	push	hl
	push	bc
	xor	a, e
	ld	d, a
	push	de
	push	bc
	xor	a, e
	call	__fpupop1
	ex	(sp), hl
	ld	d, e
	ld	e, a
	call	__fpupop2
	inc	e
	jq	z, .nonfinite1
	inc	d
	jq	z, .return2
	ld	a, d
	sub	a, e
	jq	z, .rounded
	jq	nc, .sorted
	inc	c
	ex	(sp), hl
	ld	a, e
	sub	a, d
	ld	d, e
.sorted:
	cp	a, 26
	jq	nc, .largest
	; Extend to 32 bits and shift left by ~(amount - 1) & 7
	dec	a
	ld	b, a
	xor	a, a
	srl	b
	jq	c, .noshift1
	add	hl, hl
	rla
.noshift1:
	srl	b
	jq	c, .noshift2
	repeat	2
	add	hl, hl
	rla
	end repeat
.noshift2:
	srl	b
	jq	c, .noshift4
	repeat	4
	add	hl, hl
	rla
	end repeat
.noshift4:
	; Shift right by (amount + 7) / 8 * 8, truncating to 24 bits
	; The last 2 bits shifted out are in A, while any remaining non-zero
	; bits are aggregated into the lower bits of A
	push	af
	inc	sp
	push	hl
	inc	sp
	ld	a, l
	jq	nz, .shift16
	; Shift by 8 for amounts between 1 and 8
	pop	hl
	inc	sp
	jq	.rounded

.shift16:
	; Shift by 16 for amounts between 9 and 16
	ld	e, h
	inc	sp
	pop	hl
	dec	b
	jq	z, .flush
.shift8more:
	; Shift by 24 for amounts between 17 and 24
	or	a, e
	ld	e, l
	ld	l, h
	ld	h, 0
	; Shift by 32 for amount of 25
	djnz	.shift8more
.flush:
	sub	a, 1
	sbc	a, a
	inc	a
	or	a, e
.rounded:
	ld	b, d
	pop	de
	ex	(sp), hl
	add.s	hl, hl
	jq	nc, .add
	ld	l, h
	add	hl, bc
	ld	c, l
	pop	hl
	djnz	.subtract	;always taken

.nonfinite1:
	inc	d
	jq	z, .nonfinite
.return1:
	pop	bc
.return1.pop1:
	pop	de
	ld	a, d
.return1.pop2:
	xor	a, e
	pop	bc
.return:
	pop	hl
	pop	de
	ret

.largest:
	ld	a, d
	cp	a, e
	jq	z, .return1
.return2:
	pop	bc
.return2.pop1:
	pop	de
.return2.pop2:
	ld	a, e
	pop	bc
	pop	bc
	push	bc
	jq	.return

.add:
	ld	c, h
	pop	hl
	add	hl, de
	dec	b
	jq	nc, .done
	ex	de, hl
	sbc	hl, hl
	add	hl, sp
	push	de
	rr	(hl)
	pop	hl
	rr	h
	rr	l
	rra
	jq	nc, .flushed2
	or	a, 1
.flushed2:
	inc	b
	inc	b
	jq	z, .infinite
	djnz	.done	;always taken

.borrow:
	inc	c
	add	hl, de
.subtract:
	ex	de, hl
	neg
	sbc	hl, de
	jq	c, .borrow
	jq	nz, .done
	or	a, a
.done:
	ld	de, 0800000h
	call	nz, __fppack
	pop	bc
	ex	(sp), hl
	pop	bc
	pop	de
	ret

.nonfinite:
	xor	a, a
	sbc	hl, bc
	jq	nz, .return1
	pop	hl
	sbc	hl, bc
	jq	nz, .return2.pop1
	pop	de
	bit	7, d
	jq	z, .return1.pop2
	ld	bc, 0C00000h
.infinite:
	ld	a, c
	ld	c, b ;0, also note BCU=080h from __fpupop1 or 0C0h from .nonfinite
	rrca
	or	a, 07Fh
	pop	hl
	pop	hl
	pop	de
	ret

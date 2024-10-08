; IEEE single precision unordered comparison
; z = euhl == aubc || isunordered(euhl, aubc)
; c = !isunordered(euhl, aubc)
__fpcmpu: ; CHECK: out.flags.C == !isunordered(bitcast(float, pair8_24_t, { in.HL, in.E }), bitcast(float, pair8_24_t, { in.BC, in.A })) && out.flags.Z == (!out.flags.C || (bitcast(float, pair8_24_t, { in.HL, in.E }) == bitcast(float, pair8_24_t, { in.BC, in.A }))) && out.A == in.A && out.BC == in.BC && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	call	__fpcmpo
	ret	c	; If euhl < aubc: C = 1, Z = 0
	scf
	ret	p	; If euhl >= aubc: C = 1, Z = (euhl == aubc)
	cp	a, a	; If unordered: C = 0, Z = 1
	ret

; IEEE single precision ordered comparison
; z = euhl == aubc
; c = euhl < aubc
; s = !(euhl >= aubc)
__fpcmpo: ; CHECK: out.flags.Z == (bitcast(float, pair8_24_t, { in.HL, in.E }) == bitcast(float, pair8_24_t, { in.BC, in.A })) && out.flags.C == (bitcast(float, pair8_24_t, { in.HL, in.E }) < bitcast(float, pair8_24_t, { in.BC, in.A })) && out.flags.S == !(bitcast(float, pair8_24_t, { in.HL, in.E }) >= bitcast(float, pair8_24_t, { in.BC, in.A })) && out.A == in.A && out.BC == in.BC && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	xor	a, e
	jq	z, .maybeEqual
	cp	a, 080h
	jq	c, .signsMatch
	jq	z, .maybeBothZero
	xor	a, e
	; Check if first operand could hold NaN, and if so, the second cannot on this code path
	inc	e
	jq	z, .checkFirstNan
	jq	pe, .checkFirstNan
	dec	e
.secondLargerAbs:
	cp	a, 07Fh	; C = S = !sign(aubc), Z = 0
	ret	c	; Return if A less than 07Fh
	ret	pe	; Return if A between 080h and 0FEh
.checkSecondNan:
	push	hl
	ld	hl, 07FFFFFh
	add	hl, bc
	pop	hl
	jq	c, .unordered
	cp	a, 080h	; C = S = !sign(aubc), Z = 0
	ret

.signsMatch:
	xor	a, e
	; Compare upper 7 exponent bits, which are not equal
	cp	a, e
	jq	nc, .secondLargerAbs
	inc	e
	jq	z, .checkFirstNan
	dec	e	; S = sign(euhl), Z = 0 because E > A
	rlca
	rrca		; C = sign(aubc), same as sign(euhl) on this code path
	ret	po	; Return if E != 07Fh
.checkFirstNanInc:
	inc	e
.checkFirstNan:
	ex	de, hl
	push	hl
	ld	hl, 07FFFFFh
	add	hl, de
	pop	hl
	ex	de, hl
	dec	e	; S = sign(euhl), Z = 0 because (E & 07Fh) == 07Fh
	jq	c, .unordered
	ret	p	; C = sign(euhl)
	scf	; C = sign(euhl)
	ret

.maybeBothZero:
	; Upper 7 bits of exponents are equal, but sign differs
	xor	a, e
	; Check if upper 7 bits of both exponents are zero
	add	a, a
	rra
	jq	nz, .notBothZero
	; Check if low bit of both exponents and entire mantissas are 0
	adc	hl, bc
	jq	nz, .notBothZeroFixup
	ret	nc	; Both inputs are zero, return Z=1, C=0, S=0
.notBothZeroFixup:
	or	a, a
	sbc	hl, bc
.notBothZero:
	; Check if upper 7 bits of both exponents are one
	cp	a, 07Fh	; C = S = !sign(aubc), Z = 0
	ret	c	; Return if less than 07Fh
	ret	pe	; Return if between 080h and 0FEh
	; Check the larger exponent/mantissa for NaN
	sbc	hl, bc
	add	hl, bc
	jq	nc, .checkFirstNanInc
	jq	.checkSecondNan

.maybeEqual:
	; Sign and upper 7 exponent bits are equal
	ld	a, e
	inc	a
	add	a, a
	ld	a, e
	jq	z, .checkBothNan
	or	a, a
.checkBothNanDone:
	; Compare mantissas and low exponent bit
	sbc	hl, bc
	add	hl, bc
	ret	z	; Both inputs are equal, return Z=1, C=0, S=0
	; XOR the carry with the input sign and place into the output sign/carry
	sbc	a, a
	xor	a, e
	or	a, 07Fh ; Affect S flag, Z = 0
	rlca	; Affect C flag
	ld	a, e
	ret

.checkBothNan:
	ex	de, hl
	push	hl
	ld	hl, 07FFFFFh
	add	hl, de
	jq	c, .gotFirstNan
	sbc	hl, de
	add	hl, bc
.gotFirstNan:
	pop	hl
	ex	de, hl
	jq	nc, .checkBothNanDone
.unordered:
	; Carry is always set here
	rr	a ; Z = 0, S = 1
	rla	; Restore A
	ccf	; C = 0
	ret

; IEEE single precision comparison (flag outputs for NaN inputs are undefined)
; z = euhl == aubc
; s = euhl < aubc = !(euhl >= aubc)
__fpcmp: ; CHECK: (isunordered(bitcast(float, pair8_24_t, { in.HL, in.E }), bitcast(float, pair8_24_t, { in.BC, in.A })) || (out.flags.Z == (bitcast(float, pair8_24_t, { in.HL, in.E }) == bitcast(float, pair8_24_t, { in.BC, in.A })) && out.flags.S == (bitcast(float, pair8_24_t, { in.HL, in.E }) < bitcast(float, pair8_24_t, { in.BC, in.A })))) && out.A == in.A && out.BC == in.BC && out.DE == in.DE && out.HL == in.HL && out.IX == in.IX && out.IY == in.IY
	sub	a, e
	jq	z, .maybeEqual
	cp	a, 080h
	jq	z, .maybeBothZero
	; Compare sign and upper 7 exponent bits, which are not equal
	add	a, e
	inc	e
	dec	e	; S = sign(euhl), Z = 0 because E > A
	ret	c
.notBothZero:
	cp	a, 080h	; S = !sign(aubc)
	ret	nz
	cp	a, e ; S = !sign(aubc), Z = 0 because A == 080h and A > E and A - E != 080h
	ret

.maybeBothZero:
	add	a, e
	add	a, a
	rra
	jq	nz, .notBothZero
	adc	hl, bc
	jq	c, .notBothZeroFixup
	ret	z	; Both inputs are zero, return Z=1, S=0
.notBothZeroFixup:
	or	a, a
	sbc	hl, bc
	dec	a	; S = !sign(aubc), Z = 0 because (A & 07Fh) == 0
	cpl	; Restore A, preserve S and Z
	ret

.maybeEqual:
	ld	a, e
	sbc	hl, bc
	add	hl, bc
	ret	z
	; XOR the carry with the input sign and place into the output sign
	sbc	a, a
	xor	a, e
	or	a, 07Fh ; Affect S flag, Z = 0
	ld	a, e
	ret

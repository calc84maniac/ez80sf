; IEEE single precision to 64-bit integers
; bcudeuhl = longlong(euhl)
__fptoll: ; PREREQ: bitcast(float, pair8_24_t, { in.HL, in.E })-INT64_MIN > -1.0f && bitcast(float, pair8_24_t, { in.HL, in.E }) < (INT64_MAX/2+1)*2.0f CHECK: bitcast(int64_t, tuple16_24_24_t, { out.HL, out.DE, out.BCS }) == (int64_t)bitcast(float, pair8_24_t, { in.HL, in.E }) && out.A == in.A && out.IX == in.IX && out.IY == in.IY
__fptoull: ; PREREQ: bitcast(float, pair8_24_t, { in.HL, in.E }) > -1.0f && bitcast(float, pair8_24_t, { in.HL, in.E }) < (UINT64_MAX/2+1)*2.0f CHECK: bitcast(uint64_t, tuple16_24_24_t, { out.HL, out.DE, out.BCS }) == (uint64_t)bitcast(float, pair8_24_t, { in.HL, in.E }) && out.A == in.A && out.IX == in.IX && out.IY == in.IY
	ld	d, a
	push	de
	call	__fpupop1
	ld	a, e
	ld	de, 0
	sub	a, 07Fh
	cp	a, 23 + 64
	jq	nc, .return.zero
	sub	a, 23 + 1
	jq	nc, .left
	cpl
	ld	c, a
	call	__ishru
	ld	c, b
	jq	.finish
.left:
	inc	a
	push	af
	inc	sp
	call	__llshl
	inc	sp
	inc	sp
.finish:
	pop	af
	ret	p
	jq	__llneg

.return.zero:
	sbc	hl, hl
	pop	af
	ret

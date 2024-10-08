__fpupop1:
	ld	bc, 0800000h
__fpupop2:
	sla	e
	jq	z, .denormal
	add	hl, bc
	ret	nc
	add	hl, bc
.denormal:
	inc	e
	ret

__fppack.normalize:
	dec	b
	call	nz, .entry
__fppack:
	add	hl, de
	jq	nc, .normalize
.normalized:
	rrc	l
	rlc	l
	adc	a, 07Fh
	adc	hl, de
	ld	a, b
	adc	a, e
	srl	c
	rra
	srl	b
	ret	c
	add	hl, de
	ret

.normalize.entry:
	add	a, a
	adc	hl, hl
	ret	m
	jq	nz, .normalize.continue
	or	a, a
	jq	nz, .normalize.continue
	ld	b, 1
.normalize.loop:
	add	a, a
	adc	hl, hl
	ret	m
.normalize.continue:
	djnz	.normalize.loop
	add	hl, de
	ret

__fppack2.normalize:
	dec	b
	call	nz, .loop
__fppack2:
	add	hl, de
	jq	nc, .normalize
.normalized:
	ld	a, b
	add	ix, de
	jq	nc, .rounded
	dec	ix
	add	ix, de
	jq	nc, .round
	bit	0, l
	jq	z, .rounded
.round:
	scf
	adc	hl, de
	adc	a, e
.clear:
	add	hl, de
	jq	nc, .clear
.rounded:
	cp	a, 0FFh
	jq	z, .infinite
	sla	c
	rra
	ret	nc
	add	hl, de
	ret
.infinite:
	sla	c
	rra
	ex	de, hl
	ret

.normalize.loop:
	add	ix, ix
	adc	hl, hl
	ret	m
	djnz	.normalize.loop
	add	hl, de
	ret

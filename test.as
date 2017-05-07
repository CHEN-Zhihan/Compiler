	push	sp
	push	2
	add
	pop	sp
	push	0
	pop	sb[0]
	push	0
	pop	sb[1]
L000:
	push	sb[1]
	push	10
	compNE
	j0	L001
	push	sb[1]
	puti
	push	sb[0]
	push	1
	sub
	pop	sb[0]
	push	sb[0]
L002:
	push	sb[1]
	push	1
	add
	pop	sb[1]
	push	sb[1]
	jmp	L000
L001:
	push	sb[0]
	puti
	end

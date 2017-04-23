	push	sp
	push	1
	add
	pop	sp
	push	5
	pop	sb[0]
	push	sb[0]
	push	5
	call	L000, 2
	puti
	push	sb[0]
	puti
	end
L001:
	push	fp[-5]
	push	fp[-4]
	add
	ret
L000:
	push	20
	pop	fp[-5]
	push	sb[0]
	push	fp[-5]
	call	L001, 2
	pop	sb[0]
	push	sb[0]
	ret

	push	sp
	push	1
	add
	pop	sp
	call	L000, 0
	push	sb[0]
	puti
	end
L000:
	push	100
	pop	sb[0]
	push	0
	ret

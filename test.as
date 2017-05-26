	push	sp
	push	56
	add
	pop	sp
	push	0
	pop	sb[50]
	push	1
	push	2
	push	51
	add
	pop	ac
	pop	sb[ac]
	push	51
	call	L000, 1
	end
L001:
	push	2
	push	fp[-4]
	add
	pop	ac
	push	sb[ac]
	puti
	push	3
	push	2
	push	fp[-4]
	add
	pop	ac
	pop	sb[ac]
	push	0
	ret
L000:
	push	fp[-4]
	call	L001, 1
	push	0
	ret

	push	sp
	push	13
	add
	pop	sp
	push	0
	pop	sb[0]
	push	1
	pop	sb[1]
	push	2
	pop	sb[2]
	push	sb[0]
	pop	sb[3]
	push	sb[0]
	pop	sb[4]
	push	sb[0]
	pop	sb[5]
	push	sb[0]
	pop	sb[6]
	push	sb[0]
	pop	sb[7]
	push	sb[0]
	pop	sb[8]
	push	sb[0]
	pop	sb[9]
	push	sb[0]
	pop	sb[10]
	push	sb[0]
	pop	sb[11]
	push	1
	call	L000, 1
	end
L000:
	push	fp[-4]
	j0	L001
	push	2
	neg
	pop	sb[0]
	push	sb[0]
	ret
	jmp	L002
L001:
	push	2
	pop	sb[12]
	push	sb[12]
	ret
L002:
	push	0
	ret

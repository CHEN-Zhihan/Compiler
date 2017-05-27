	push	sp
	push	40
	add
	pop	sp
	push	5
	pop	sb[0]
	push	5
	pop	sb[1]
	push	5
	pop	sb[2]
	push	5
	pop	sb[3]
	push	5
	pop	sb[4]
	push	5
	pop	sb[5]
	push	5
	pop	sb[6]
	push	5
	pop	sb[7]
	push	5
	pop	sb[8]
	push	5
	pop	sb[9]
	push	5
	pop	sb[10]
	push	5
	pop	sb[11]
	push	5
	pop	sb[12]
	push	5
	pop	sb[13]
	push	5
	pop	sb[14]
	push	5
	pop	sb[15]
	push	5
	pop	sb[16]
	push	5
	pop	sb[17]
	push	5
	pop	sb[18]
	push	5
	pop	sb[19]
	push	5
	pop	sb[20]
	push	5
	pop	sb[21]
	push	5
	pop	sb[22]
	push	5
	pop	sb[23]
	push	5
	pop	sb[24]
	push	3
	pop	sb[25]
	push	3
	pop	sb[26]
	push	3
	pop	sb[27]
	push	3
	pop	sb[28]
	push	3
	pop	sb[29]
	push	3
	pop	sb[30]
	push	3
	pop	sb[31]
	push	3
	pop	sb[32]
	push	3
	pop	sb[33]
	push	3
	pop	sb[34]
	push	3
	pop	sb[35]
	push	3
	pop	sb[36]
	push	3
	pop	sb[37]
	push	3
	pop	sb[38]
	push	3
	pop	sb[39]
	push	0
	push	5
	call	L000, 2
	push	25
	push	3
	call	L000, 2
	end
L000:
	push	1
	push	sp
	add
	pop	sp
	push	0
	pop	fp[0]
L001:
	push	fp[0]
	push	fp[-4]
	compNE
	j0	L002
	push	fp[0]
	push	5
	mul
	push	2
	add
	push	fp[-5]
	add
	pop	ac
	push	sb[ac]
	puti
L003:
	push	fp[0]
	push	1
	add
	pop	fp[0]
	jmp	L001
L002:
	push	0
	ret

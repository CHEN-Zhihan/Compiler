	push	sp
	push	15
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
	call	L000, 0
	call	L001, 0
	end
L001:
	push	0
	pop	sb[12]
	push	sb[12]
	pop	sb[13]
L002:
	push	1
	j0	L003
	geti
	pop	sb[14]
	push	sb[14]
	push	3
	div
	pop	sb[13]
	push	sb[14]
	push	3
	mod
	pop	sb[12]
	push	sb[13]
	push	3
	mul
	push	sb[12]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[0]
	compNE
	j0	L004
	push	"Cannot put your mark here!"
	puts
	jmp	L005
L004:
	jmp	L003
L005:
	jmp	L002
L003:
	push	sb[1]
	push	sb[13]
	push	3
	mul
	push	sb[12]
	add
	push	3
	add
	pop	ac
	pop	sb[ac]
	push	0
	ret
L000:
	push	0
	pop	sb[12]
L006:
	push	sb[12]
	push	3
	compNE
	j0	L007
	push	0
	pop	sb[13]
L009:
	push	sb[13]
	push	3
	compNE
	j0	L010
	push	sb[12]
	push	3
	mul
	push	sb[13]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	puti_
	push	" "
	puts_
L011:
	push	sb[13]
	push	1
	add
	pop	sb[13]
	jmp	L009
L010:
	push	""
	puts
L008:
	push	sb[12]
	push	1
	add
	pop	sb[12]
	jmp	L006
L007:
	push	0
	ret

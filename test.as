	push	sp
	push	13
	add
	pop	sp
	push	0
	pop	sb[11]
L000:
	push	sb[11]
	push	2
	compNE
	j0	L001
	push	0
	pop	sb[12]
L003:
	push	sb[12]
	push	5
	compNE
	j0	L004
	geti
	push	sb[11]
	push	5
	mul
	push	sb[12]
	add
	push	1
	add
	pop	ac
	pop	sb[ac]
L005:
	push	sb[12]
	push	1
	add
	pop	sb[12]
	jmp	L003
L004:
L002:
	push	sb[11]
	push	1
	add
	pop	sb[11]
	jmp	L000
L001:
	push	0
	pop	sb[11]
L006:
	push	sb[11]
	push	2
	compNE
	j0	L007
	push	0
	pop	sb[12]
L009:
	push	sb[12]
	push	5
	compNE
	j0	L010
	push	sb[11]
	push	5
	mul
	push	sb[12]
	add
	push	1
	add
	pop	ac
	push	sb[ac]
	puti_
	push	"  "
	puts_
L011:
	push	sb[12]
	push	1
	add
	pop	sb[12]
	jmp	L009
L010:
	push	""
	puts
L008:
	push	sb[11]
	push	1
	add
	pop	sb[11]
	jmp	L006
L007:
	end

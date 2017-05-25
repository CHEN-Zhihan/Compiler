	push	sp
	push	13
	add
	pop	sp
	push	0
	pop	sb[12]
L000:
	push	sb[12]
	push	5
	compNE
	j0	L001
	geti
	push	sb[12]
	push	1
	add
	pop	sb
	pop	sb[sb]
L002:
	push	sb[12]
	push	1
	add
	pop	sb[12]
	jmp	L000
L001:
	push	0
	pop	sb[12]
L003:
	push	sb[12]
	push	5
	compNE
	j0	L004
	push	sb[12]
	push	1
	add
	pop	sb
	push	sb[sb]
	pop	sb[8]
	push	sb[12]
	pop	sb[9]
	push	sb[12]
	push	1
	add
	pop	sb[10]
L006:
	push	sb[10]
	push	5
	compLT
	j0	L007
	push	sb[10]
	push	1
	add
	pop	sb
	push	sb[sb]
	push	sb[8]
	compGT
	j0	L009
	push	sb[10]
	push	1
	add
	pop	sb
	push	sb[sb]
	pop	sb[8]
	push	sb[10]
	pop	sb[9]
L009:
L008:
	push	sb[10]
	push	1
	add
	pop	sb[10]
	jmp	L006
L007:
	push	sb[12]
	push	1
	add
	pop	sb
	push	sb[sb]
	pop	sb[11]
	push	sb[8]
	push	sb[12]
	push	1
	add
	pop	sb
	pop	sb[sb]
	push	sb[11]
	push	sb[9]
	push	1
	add
	pop	sb
	pop	sb[sb]
L005:
	push	sb[12]
	push	1
	add
	pop	sb[12]
	jmp	L003
L004:
	push	0
	pop	sb[12]
L010:
	push	sb[12]
	push	5
	compNE
	j0	L011
	push	sb[12]
	push	1
	add
	pop	sb
	push	sb[sb]
	puti
L012:
	push	sb[12]
	push	1
	add
	pop	sb[12]
	jmp	L010
L011:
	end

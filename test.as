	push	sp
	push	2
	add
	pop	sp
	call	L000, 0
	call	L001, 0
	end
L001:
L002:
	push	1
	j0	L003
	push	1
	j0	L004
	jmp	L003
L004:
	jmp	L002
L003:
	push	0
	ret
L000:
	push	0
	pop	sb[0]
L005:
	push	sb[0]
	push	3
	compNE
	j0	L006
	push	0
	pop	sb[1]
L008:
	push	sb[1]
	push	3
	compNE
	j0	L009
	push	1
	puti
L010:
	push	sb[1]
	push	1
	add
	pop	sb[1]
	jmp	L008
L009:
L007:
	push	sb[0]
	push	1
	add
	pop	sb[0]
	jmp	L005
L006:
	push	0
	ret

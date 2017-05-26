	push	sp
	push	2
	add
	pop	sp
	push	1
	call	L000, 1
	end
L001:
	push	1
	j0	L002
L002:
	push	0
	ret
L000:
	call	L001, 0
	push	0
	pop	sb[0]
L003:
	push	sb[0]
	push	3
	compNE
	j0	L004
	push	0
	pop	sb[1]
L006:
	push	sb[1]
	push	3
	compNE
	j0	L007
	push	"I am computer, putting at "
	puts
L008:
	push	sb[1]
	push	1
	add
	pop	sb[1]
	jmp	L006
L007:
L005:
	push	sb[0]
	push	1
	add
	pop	sb[0]
	jmp	L003
L004:
	push	0
	ret

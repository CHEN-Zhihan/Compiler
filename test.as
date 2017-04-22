	push	1
	push	2
	call	L000, 2
	puti
	push	1
	push	2
	push	3
	call	L001, 3
	puti
	jmp	L999
L001:
	push	fp[-6]
	push	fp[-5]
	add
	push	fp[-4]
	add
	ret
L000:
	push	fp[-5]
	push	fp[-4]
	add
	ret
L999:

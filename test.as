	push	6
	push	4
	call	L000, 2
	puti
	end
L000:
	push	fp[-5]
	push	5
	compEQ
	j0	L001
	push	4
	ret
L001:
	push	fp[-5]
	push	1
	sub
	push	fp[-4]
	call	L000, 2
	ret

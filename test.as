	push	sp
	push	3
	add
	pop	sp
	push	5
	pop	sb[0]
	push	2
	pop	sb[1]
	push	sb[0]
	push	1
	compEQ
	j0	L000
	push	3
	pop	sb[2]
	push	sb[0]
	push	sb[2]
	add
	pop	sb[1]
	push	sb[1]
	puti
	push	"gg"
	puts
	jmp	L001
L000:
	push	4
	pop	sb[2]
	push	sb[0]
	push	sb[2]
	add
	pop	sb[1]
	push	sb[1]
	puti
L001:
	push	sb[1]
	puti
	end

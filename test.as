	push	"gg"
	push	1
	push	sb[1]
	push	1
	add
	pop	sb[1]
	push	sb[1]
	push	2
	compEQ
	j0	L000
	push	3
L000:

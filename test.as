	push	sp
	push	13
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
	push	0
	pop	sb[12]
	push	1
	call	L000, 1
	push	sb[12]
	puti
	end
L001:
	push	1
	push	sp
	add
	pop	sp
	push	0
	push	3
	add
	pop	ac
	push	sb[ac]
	push	4
	push	3
	add
	pop	ac
	push	sb[ac]
	compEQ
	push	4
	push	3
	add
	pop	ac
	push	sb[ac]
	push	8
	push	3
	add
	pop	ac
	push	sb[ac]
	compEQ
	and
	j0	L003
	push	0
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[1]
	compEQ
	j0	L004
	push	1
	neg
	ret
L004:
	push	0
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[2]
	compEQ
	j0	L005
	push	1
	ret
L005:
L003:
	push	6
	push	3
	add
	pop	ac
	push	sb[ac]
	push	4
	push	3
	add
	pop	ac
	push	sb[ac]
	compEQ
	push	4
	push	3
	add
	pop	ac
	push	sb[ac]
	push	2
	push	3
	add
	pop	ac
	push	sb[ac]
	compEQ
	and
	j0	L006
	push	6
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[1]
	compEQ
	j0	L007
	push	1
	neg
	ret
	jmp	L008
L007:
	push	6
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[2]
	compEQ
	j0	L009
	push	1
	ret
L009:
L008:
L006:
	push	0
	pop	fp[0]
L010:
	push	fp[0]
	push	3
	compNE
	j0	L011
	push	fp[0]
	push	3
	mul
	push	0
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	fp[0]
	push	3
	mul
	push	1
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	compEQ
	push	fp[0]
	push	3
	mul
	push	1
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	fp[0]
	push	3
	mul
	push	2
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	compEQ
	and
	j0	L013
	push	fp[0]
	push	3
	mul
	push	0
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[2]
	compEQ
	j0	L014
	push	1
	ret
L014:
	push	fp[0]
	push	3
	mul
	push	0
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[1]
	compEQ
	j0	L015
	push	1
	neg
	ret
L015:
L013:
L012:
	push	fp[0]
	push	1
	add
	pop	fp[0]
	jmp	L010
L011:
	push	0
	pop	fp[0]
L016:
	push	fp[0]
	push	3
	compNE
	j0	L017
	push	0
	push	fp[0]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	3
	push	fp[0]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	compEQ
	push	3
	push	fp[0]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	6
	push	fp[0]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	compEQ
	and
	j0	L019
	push	0
	push	fp[0]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[2]
	compEQ
	j0	L020
	push	1
	ret
L020:
	push	0
	push	fp[0]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[1]
	compEQ
	j0	L021
	push	1
	neg
	ret
L021:
L019:
L018:
	push	fp[0]
	push	1
	add
	pop	fp[0]
	jmp	L016
L017:
	push	0
	ret
	push	0
	ret
L002:
	push	2
	push	sp
	add
	pop	sp
	push	0
	pop	fp[0]
L022:
	push	fp[0]
	push	3
	compNE
	j0	L023
	push	0
	pop	fp[1]
L025:
	push	fp[1]
	push	3
	compNE
	j0	L026
	push	fp[0]
	push	3
	mul
	push	fp[1]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[0]
	compEQ
	j0	L028
	push	1
	ret
L028:
L027:
	push	fp[1]
	push	1
	add
	pop	fp[1]
	jmp	L025
L026:
L024:
	push	fp[0]
	push	1
	add
	pop	fp[0]
	jmp	L022
L023:
	push	0
	ret
	push	0
	ret
L000:
	push	4
	push	sp
	add
	pop	sp
	call	L001, 0
	pop	fp[0]
	push	fp[0]
	push	0
	compNE
	j0	L029
	push	fp[0]
	ret
L029:
	push	1
	call	L002, 0
	sub
	j0	L030
	push	1
	neg
	pop	fp[1]
	push	0
	ret
L030:
	push	fp[-4]
	j0	L031
	push	2
	neg
	pop	fp[1]
	push	0
	pop	fp[2]
L032:
	push	fp[2]
	push	3
	compNE
	j0	L033
	push	0
	pop	fp[3]
L035:
	push	fp[3]
	push	3
	compNE
	j0	L036
	push	fp[2]
	push	3
	mul
	push	fp[3]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[0]
	compEQ
	j0	L038
	push	sb[2]
	push	fp[2]
	push	3
	mul
	push	fp[3]
	add
	push	3
	add
	pop	ac
	pop	sb[ac]
	push	0
	call	L000, 1
	pop	fp[0]
	push	sb[0]
	push	fp[2]
	push	3
	mul
	push	fp[3]
	add
	push	3
	add
	pop	ac
	pop	sb[ac]
	push	fp[0]
	push	fp[1]
	compGT
	j0	L039
	push	fp[0]
	pop	fp[1]
	push	fp[2]
	push	3
	mul
	push	fp[3]
	add
	pop	sb[12]
L039:
L038:
L037:
	push	fp[3]
	push	1
	add
	pop	fp[3]
	jmp	L035
L036:
L034:
	push	fp[2]
	push	1
	add
	pop	fp[2]
	jmp	L032
L033:
	push	fp[1]
	ret
	jmp	L040
L031:
	push	2
	pop	fp[1]
	push	0
	pop	fp[2]
L041:
	push	fp[2]
	push	3
	compNE
	j0	L042
	push	0
	pop	fp[3]
L044:
	push	fp[3]
	push	3
	compNE
	j0	L045
	push	fp[2]
	push	3
	mul
	push	fp[3]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[0]
	compEQ
	j0	L047
	push	sb[1]
	push	fp[2]
	push	3
	mul
	push	fp[3]
	add
	push	3
	add
	pop	ac
	pop	sb[ac]
	push	1
	call	L000, 1
	pop	fp[0]
	push	sb[0]
	push	fp[2]
	push	3
	mul
	push	fp[3]
	add
	push	3
	add
	pop	ac
	pop	sb[ac]
	push	fp[1]
	push	fp[0]
	compLT
	j0	L048
	push	fp[0]
	pop	fp[1]
	push	fp[2]
	push	3
	mul
	push	fp[3]
	add
	pop	sb[12]
L048:
L047:
L046:
	push	fp[3]
	push	1
	add
	pop	fp[3]
	jmp	L044
L045:
L043:
	push	fp[2]
	push	1
	add
	pop	fp[2]
	jmp	L041
L042:
	push	fp[1]
	ret
L040:
	push	0
	ret

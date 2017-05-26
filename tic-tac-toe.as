	push	sp
	push	16
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
	push	1
	call	L000, 1
	end
L002:
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
	j0	L004
	push	0
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[1]
	compEQ
	j0	L005
	push	1
	neg
	ret
L005:
	push	0
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[2]
	compEQ
	j0	L006
	push	1
	ret
L006:
L004:
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
	j0	L007
	push	6
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[1]
	compEQ
	j0	L008
	push	1
	neg
	ret
	jmp	L009
L008:
	push	0
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[2]
	compEQ
	j0	L010
	push	1
	ret
L010:
L009:
L007:
	push	0
	pop	sb[12]
L011:
	push	sb[12]
	push	3
	compNE
	j0	L012
	push	sb[12]
	push	3
	mul
	push	0
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[12]
	push	3
	mul
	push	1
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	compEQ
	push	sb[12]
	push	3
	mul
	push	1
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[12]
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
	j0	L014
	push	sb[12]
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
	j0	L015
	push	1
	ret
L015:
	push	sb[12]
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
	j0	L016
	push	1
	neg
	ret
L016:
L014:
L013:
	push	sb[12]
	push	1
	add
	pop	sb[12]
	jmp	L011
L012:
	push	0
	pop	sb[12]
L017:
	push	sb[12]
	push	3
	compNE
	j0	L018
	push	0
	push	sb[12]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	3
	push	sb[12]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	compEQ
	push	3
	push	sb[12]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	6
	push	sb[12]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	compEQ
	and
	j0	L020
	push	0
	push	sb[12]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[2]
	compEQ
	j0	L021
	push	1
	ret
L021:
	push	0
	push	sb[12]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[1]
	compEQ
	j0	L022
	push	1
	neg
	ret
L022:
L020:
L019:
	push	sb[12]
	push	1
	add
	pop	sb[12]
	jmp	L017
L018:
	push	0
	ret
	push	0
	ret
L001:
	push	0
	pop	sb[12]
L023:
	push	sb[12]
	push	3
	compNE
	j0	L024
	push	0
	pop	sb[13]
L026:
	push	sb[13]
	push	3
	compNE
	j0	L027
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
L028:
	push	sb[13]
	push	1
	add
	pop	sb[13]
	jmp	L026
L027:
	push	""
	puts
L025:
	push	sb[12]
	push	1
	add
	pop	sb[12]
	jmp	L023
L024:
	push	0
	ret
L003:
	push	0
	pop	sb[13]
L029:
	push	sb[13]
	push	3
	compNE
	j0	L030
	push	0
	pop	sb[14]
L032:
	push	sb[14]
	push	3
	compNE
	j0	L033
	push	sb[13]
	push	3
	mul
	push	sb[14]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[0]
	compEQ
	j0	L035
	push	1
	ret
L035:
L034:
	push	sb[14]
	push	1
	add
	pop	sb[14]
	jmp	L032
L033:
L031:
	push	sb[13]
	push	1
	add
	pop	sb[13]
	jmp	L029
L030:
	push	0
	ret
	push	0
	ret
L000:
	push	"begin"
	puts
	call	L001, 0
	call	L002, 0
	pop	sb[12]
	push	sb[12]
	puti
	push	sb[12]
	push	0
	compNE
	j0	L036
	push	sb[12]
	ret
L036:
	push	1
	call	L003, 0
	sub
	j0	L037
	push	"no empty available"
	puts
	push	0
	ret
L037:
	push	fp[-4]
	j0	L038
	push	2
	neg
	pop	sb[13]
	push	0
	pop	sb[14]
L039:
	push	sb[14]
	push	3
	compNE
	j0	L040
	push	0
	pop	sb[15]
L042:
	push	sb[15]
	push	3
	compNE
	j0	L043
	push	sb[14]
	push	3
	mul
	push	sb[15]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[0]
	compEQ
	j0	L045
	push	sb[2]
	push	sb[14]
	push	3
	mul
	push	sb[15]
	add
	push	3
	add
	pop	ac
	pop	sb[ac]
	push	"I am computer, putting at "
	puts_
	push	sb[14]
	push	3
	mul
	push	sb[15]
	add
	puti
	push	0
	call	L000, 1
	pop	sb[12]
	push	sb[0]
	push	sb[14]
	push	3
	mul
	push	sb[15]
	add
	push	3
	add
	pop	ac
	pop	sb[ac]
	push	sb[12]
	push	sb[13]
	compGT
	j0	L046
	push	sb[12]
	pop	sb[13]
L046:
L045:
L044:
	push	sb[15]
	push	1
	add
	pop	sb[15]
	jmp	L042
L043:
L041:
	push	sb[14]
	push	1
	add
	pop	sb[14]
	jmp	L039
L040:
	push	sb[13]
	ret
	jmp	L047
L038:
	push	2
	pop	sb[13]
	push	0
	pop	sb[14]
L048:
	push	sb[14]
	push	3
	compNE
	j0	L049
	push	0
	pop	sb[15]
L051:
	push	sb[15]
	push	3
	compNE
	j0	L052
	push	sb[14]
	push	3
	mul
	push	sb[15]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[0]
	compEQ
	j0	L054
	push	sb[1]
	push	sb[14]
	push	3
	mul
	push	sb[15]
	add
	push	3
	add
	pop	ac
	pop	sb[ac]
	push	"I am player, putting at "
	puts_
	push	sb[14]
	push	3
	mul
	push	sb[15]
	add
	puti
	push	1
	call	L000, 1
	pop	sb[12]
	push	sb[0]
	push	sb[14]
	push	3
	mul
	push	sb[15]
	add
	push	3
	add
	pop	ac
	pop	sb[ac]
	push	sb[13]
	push	sb[12]
	compLT
	j0	L055
	push	sb[12]
	pop	sb[13]
L055:
L054:
L053:
	push	sb[15]
	push	1
	add
	pop	sb[15]
	jmp	L051
L052:
L050:
	push	sb[14]
	push	1
	add
	pop	sb[14]
	jmp	L048
L049:
	push	sb[13]
	ret
L047:
	push	0
	ret

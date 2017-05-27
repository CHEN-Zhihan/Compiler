	push	sp
	push	12
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
	call	L000, 0
	end
L007:
	push	4
	push	sp
	add
	pop	sp
	call	L004, 0
	pop	fp[0]
	push	fp[0]
	push	0
	compNE
	j0	L008
	push	fp[0]
	ret
L008:
	push	1
	call	L005, 0
	sub
	j0	L009
	push	0
	ret
L009:
	push	fp[-4]
	j0	L010
	push	2
	neg
	pop	fp[1]
	push	0
	pop	fp[2]
L011:
	push	fp[2]
	push	3
	compNE
	j0	L012
	push	0
	pop	fp[3]
L014:
	push	fp[3]
	push	3
	compNE
	j0	L015
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
	j0	L017
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
	call	L007, 1
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
	j0	L018
	push	fp[0]
	pop	fp[1]
L018:
L017:
L016:
	push	fp[3]
	push	1
	add
	pop	fp[3]
	jmp	L014
L015:
L013:
	push	fp[2]
	push	1
	add
	pop	fp[2]
	jmp	L011
L012:
	push	fp[1]
	ret
	jmp	L019
L010:
	push	2
	pop	fp[1]
	push	0
	pop	fp[2]
L020:
	push	fp[2]
	push	3
	compNE
	j0	L021
	push	0
	pop	fp[3]
L023:
	push	fp[3]
	push	3
	compNE
	j0	L024
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
	j0	L026
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
	call	L007, 1
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
	compGT
	j0	L027
	push	fp[0]
	pop	fp[1]
L027:
L026:
L025:
	push	fp[3]
	push	1
	add
	pop	fp[3]
	jmp	L023
L024:
L022:
	push	fp[2]
	push	1
	add
	pop	fp[2]
	jmp	L020
L021:
	push	fp[1]
	ret
L019:
	push	0
	ret
L006:
	push	5
	push	sp
	add
	pop	sp
	push	2
	neg
	pop	fp[0]
	push	0
	pop	fp[1]
	push	0
	pop	fp[2]
L028:
	push	fp[2]
	push	3
	compNE
	j0	L029
	push	0
	pop	fp[3]
L031:
	push	fp[3]
	push	3
	compNE
	j0	L032
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
	j0	L034
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
	call	L007, 1
	pop	fp[4]
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
	push	fp[4]
	push	fp[0]
	compGT
	j0	L035
	push	fp[2]
	push	3
	mul
	push	fp[3]
	add
	pop	fp[1]
	push	fp[4]
	pop	fp[0]
L035:
L034:
L033:
	push	fp[3]
	push	1
	add
	pop	fp[3]
	jmp	L031
L032:
L030:
	push	fp[2]
	push	1
	add
	pop	fp[2]
	jmp	L028
L029:
	push	fp[1]
	ret
	push	0
	ret
L005:
	push	2
	push	sp
	add
	pop	sp
	push	0
	pop	fp[0]
L036:
	push	fp[0]
	push	3
	compNE
	j0	L037
	push	0
	pop	fp[1]
L039:
	push	fp[1]
	push	3
	compNE
	j0	L040
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
	j0	L042
	push	1
	ret
L042:
L041:
	push	fp[1]
	push	1
	add
	pop	fp[1]
	jmp	L039
L040:
L038:
	push	fp[0]
	push	1
	add
	pop	fp[0]
	jmp	L036
L037:
	push	0
	ret
	push	0
	ret
L004:
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
	j0	L043
	push	0
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[1]
	compEQ
	j0	L044
	push	1
	neg
	ret
L044:
	push	0
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[2]
	compEQ
	j0	L045
	push	1
	ret
L045:
L043:
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
	j0	L046
	push	6
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[1]
	compEQ
	j0	L047
	push	1
	neg
	ret
	jmp	L048
L047:
	push	6
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[2]
	compEQ
	j0	L049
	push	1
	ret
L049:
L048:
L046:
	push	0
	pop	fp[0]
L050:
	push	fp[0]
	push	3
	compNE
	j0	L051
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
	j0	L053
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
	j0	L054
	push	1
	ret
L054:
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
	j0	L055
	push	1
	neg
	ret
L055:
L053:
L052:
	push	fp[0]
	push	1
	add
	pop	fp[0]
	jmp	L050
L051:
	push	0
	pop	fp[0]
L056:
	push	fp[0]
	push	3
	compNE
	j0	L057
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
	j0	L059
	push	0
	push	fp[0]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[2]
	compEQ
	j0	L060
	push	1
	ret
L060:
	push	0
	push	fp[0]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[1]
	compEQ
	j0	L061
	push	1
	neg
	ret
L061:
L059:
L058:
	push	fp[0]
	push	1
	add
	pop	fp[0]
	jmp	L056
L057:
	push	0
	ret
	push	0
	ret
L001:
	push	4
	push	sp
	add
	pop	sp
	push	"================="
	pop	fp[0]
	push	"|| "
	pop	fp[1]
	push	" || "
	pop	fp[2]
	push	" ||"
	pop	fp[3]
	push	fp[0]
	puts
	push	fp[1]
	puts_
	push	0
	push	0
	call	L002, 2
	putc_
	push	fp[2]
	puts_
	push	0
	push	1
	call	L002, 2
	putc_
	push	fp[2]
	puts_
	push	0
	push	2
	call	L002, 2
	putc_
	push	fp[3]
	puts
	push	fp[0]
	puts
	push	fp[1]
	puts_
	push	1
	push	0
	call	L002, 2
	putc_
	push	fp[2]
	puts_
	push	1
	push	1
	call	L002, 2
	putc_
	push	fp[2]
	puts_
	push	1
	push	2
	call	L002, 2
	putc_
	push	fp[3]
	puts
	push	fp[0]
	puts
	push	fp[1]
	puts_
	push	2
	push	0
	call	L002, 2
	putc_
	push	fp[2]
	puts_
	push	2
	push	1
	call	L002, 2
	putc_
	push	fp[2]
	puts_
	push	2
	push	2
	call	L002, 2
	putc_
	push	fp[3]
	puts
	push	fp[0]
	puts
	push	0
	ret
L002:
	push	fp[-5]
	push	3
	mul
	push	fp[-4]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[0]
	compEQ
	j0	L062
	push	fp[-5]
	push	3
	mul
	push	fp[-4]
	add
	push	'0'
	add
	ret
L062:
	push	fp[-5]
	push	3
	mul
	push	fp[-4]
	add
	push	3
	add
	pop	ac
	push	sb[ac]
	push	sb[1]
	compEQ
	j0	L063
	push	'O'
	ret
L063:
	push	'X'
	ret
	push	0
	ret
L000:
	push	1
	push	sp
	add
	pop	sp
	call	L001, 0
L064:
	push	1
	j0	L065
	push	3
	call	L003, 1
	call	L004, 0
	push	1
	neg
	compEQ
	j0	L066
	push	"You win!!!"
	puts
	jmp	L065
L066:
	push	1
	call	L005, 0
	sub
	j0	L067
	push	"Draw!!!"
	puts
	jmp	L065
L067:
	call	L006, 0
	pop	fp[0]
	push	sb[2]
	push	fp[0]
	push	3
	div
	push	3
	mul
	push	fp[0]
	push	3
	mod
	add
	push	3
	add
	pop	ac
	pop	sb[ac]
	call	L001, 0
	call	L004, 0
	push	1
	compEQ
	j0	L068
	push	"You lose!!!"
	puts
	jmp	L065
L068:
	push	1
	call	L005, 0
	sub
	j0	L069
	push	"Draw!!!"
	puts
	jmp	L065
L069:
	jmp	L064
L065:
	push	0
	ret
L003:
	push	3
	push	sp
	add
	pop	sp
	push	0
	pop	fp[0]
	push	fp[0]
	pop	fp[1]
L070:
	push	1
	j0	L071
	push	"Enter your input: "
	puts_
	geti
	pop	fp[2]
	push	fp[2]
	push	3
	div
	pop	fp[1]
	push	fp[2]
	push	3
	mod
	pop	fp[0]
	push	fp[1]
	push	3
	mul
	push	fp[0]
	add
	push	fp[-4]
	add
	pop	ac
	push	sb[ac]
	push	sb[0]
	compNE
	push	fp[2]
	push	0
	compLT
	or
	push	fp[2]
	push	9
	compGE
	or
	j0	L072
	push	"Cannot put your mark here!"
	puts
	jmp	L073
L072:
	jmp	L071
L073:
	jmp	L070
L071:
	push	sb[1]
	push	fp[1]
	push	3
	mul
	push	fp[0]
	add
	push	fp[-4]
	add
	pop	ac
	pop	sb[ac]
	push	0
	ret

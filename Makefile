c6: scanner.cpp parser.tab.c compiler.cpp cvm
	g++ scanner.cpp parser.tab.c compiler.cpp -o c6 -std=c++11 -g

run:c6
	./c6 test.hc > test.as
	./cvm test.as

scanner.cpp: scanner.l
	flex  -o scanner.cpp scanner.l

parser.tab.c: parser.y
	bison parser.y -d

cvm: nas/nas.l nas/nas.y
	flex  -o nas.yy.c nas/nas.l
	bison nas/nas.y -d
	g++ nas.yy.c nas.tab.c -o cvm -std=c++11
	rm nas.tab.c nas.tab.h nas.yy.c

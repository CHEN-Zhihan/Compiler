c6: scanner.cpp parser.tab.c compiler.cpp cvm Node.cpp Node.h
	g++ scanner.cpp parser.tab.c compiler.cpp Node.cpp -o c6 -std=c++11 -g

compile: c6 test.sc
	./c6 test.sc

run: c6 test.sc cvm
	./c6 test.sc > test.as
	./cvm test.as

play: c6 tic-tac-toe.sc cvm
	./c6 tic-tac-toe.sc > tic-tac-toe.as
	./cvm tic-tac-toe.as

scanner.cpp: scanner.l
	flex  -o scanner.cpp scanner.l

parser.tab.c: parser.y
	bison parser.y -d

clean:
	rm scanner.cpp parser.tab.h parser.tab.c cvm c6

cvm: nas/nas.l nas/nas.y
	flex  -o nas.yy.c nas/nas.l
	bison nas/nas.y -d
	g++ nas.yy.c nas.tab.c -o cvm -std=c++11 -g
	rm nas.tab.c nas.tab.h nas.yy.c

.PHONY:
	clean

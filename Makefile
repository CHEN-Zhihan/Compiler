c6: c6.yy.c c6.tab.c cvm
	g++ c6.yy.c c6.tab.c c6c.cpp -o c6 -std=c++11 -g
	rm c6.yy.c c6.tab.c c6.tab.h

c6.yy.c: c6.l
	flex  -o c6.yy.c c6.l

c6.tab.c: c6.y
	bison c6.y -d

cvm: nas/nas.l nas/nas.y
	flex  -o nas.yy.c nas/nas.l
	bison nas/nas.y -d
	g++ nas.yy.c nas.tab.c -o cvm -std=c++11
	rm nas.tab.c nas.tab.h nas.yy.c

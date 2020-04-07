plcc: scan.l parser.y
	bison -d parser.y
	flex scan.l
	gcc -o $@ parser.tab.c lex.yy.c -lfl

.PHONY: clean
clean:
	rm -rf plcc *.tab.c *.tab.h *.yy.c

CXX=g++
FLAGS=-std=c++11 -Wall
LIBS=-lfl

INCLUDE=-I include
SRC=src

%.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@

plcc: $(SRC)
	bison -d $(SRC)/parser.y
	flex $(SRC)/scanner.l
	$(CXX) $(FLAGS) -o $@ parser.tab.c lex.yy.c $(INCLUDE) $(LIBS)

.PHONY: clean
clean:
	rm -rf plcc *.tab.c *.tab.h *.yy.c

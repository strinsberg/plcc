CXX=g++
FLAGS=-std=c++11 -Wall
LIBS=-lfl

INCLUDE=-I include
SRC=src

COMPILER=plcc

%.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@

$(COMPILER): $(SRC)
	bison -d $(SRC)/parser.y
	flex $(SRC)/scanner.l
	mv parser.tab.hh include/parser.tab.hh; mv stack.hh include/stack.hh
	mv parser.tab.cc src/parser.tab.cpp; mv lex.yy.c src/lex.yy.cpp
	$(CXX) $(FLAGS) -o $@ $(SRC)/*.cpp $(INCLUDE) $(LIBS)

.PHONY: tests
tests: $(COMPILER)
	./plcc -v test/test2.pal

.PHONY: clean
clean:
	rm -rf plcc src/*.tab.cpp src/*.yy.cpp include/parser.tab.hh include/stack.hh \
	*.byte *.asm *.out *.dasm

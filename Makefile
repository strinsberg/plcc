CXX=g++
FLAGS=-std=c++11 -Wall
LIBS=-lfl

INCLUDE=-I include
SRC=src

COMPILER=plcc

%.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(FLAGS) -c $< -o $@

$(COMPILER): $(SRC)
	bison -d $(SRC)/parser.y
	flex $(SRC)/scanner.l
	mv parser.tab.hh include/parser.tab.hh
	mv stack.hh include/stack.hh
	$(CXX) $(FLAGS) -o $@ $(SRC)/*.cpp parser.tab.cc lex.yy.c $(INCLUDE) $(LIBS)

.PHONY: tests
tests: $(COMPILER)
	./plcc < test/test2.pal

.PHONY: clean
clean:
	rm -rf plcc *.tab.cc *.tab.hh *.yy.c include/parser.tab.hh include/stack.hh

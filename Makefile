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
	$(CXX) $(FLAGS) -o $@ $(SRC)/*.cpp parser.tab.c lex.yy.c $(INCLUDE) $(LIBS)

.PHONY: tests
tests: $(COMPILER)
	./plcc < test/test2.pal

.PHONY: clean
clean:
	rm -rf plcc *.tab.c *.tab.h *.yy.c

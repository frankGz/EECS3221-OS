HDR = eval.h proc.h
SRC = y.tab.c forklib.c util.c eval.c main.c
OBJ = y.tab.o forklib.o util.o eval.o main.o
SRC = y.tab.c forklib.c util.c secreteval.c main.c
OBJ = y.tab.o forklib.o util.o secreteval.o main.o
LEX = flex -l
YACC = bison -y
LEXFILE = shlex.l
YACCFILE = shyacc.y
CFLAGS= -c -g

mysh: $(OBJ)
	gcc -o mysh $(OBJ)

y.tab.o:  y.tab.c

$(SRC): $(HDR)

y.tab.c: lex.yy.c $(YACCFILE)
	yacc -v $(YACCFILE)

lex.yy.c: $(LEXFILE)
	$(LEX) $(LEXFILE)

clean:
	rm -f y.tab.c lex.yy.c mysh y.tab.o forklib.o util.o secreteval.o main.o

tar:
	tar cf mysh.tar forklib.c util.c eval.h eval.c main.c shlex.l shyacc.y mysh-manual.ascii proc.h makefile

soltar:
	tar cf mysh.tar forklib.c util.c eval.c eval.h secreteval.c main.c shlex.l shyacc.y mysh-manual.ascii proc.h makefile

man:
	less mysh-manual.ascii

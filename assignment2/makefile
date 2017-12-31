##  Author: Minas Spetsakis
##  Descrn: A thread performance evaluation program.
##  Both programming components of the assignment are in this makefile
##  but two makefiles in two directories is fine too.
##  Date  :  Nov. 19 2017

SRC1 = main1.c runner.c error.c util.c
OBJ1 = main1.o runner.o error.o util.o
SRC2 = main2.c color.c error.c util.c
OBJ2 = main2.o color.o error.o util.o
SRC  = ${SRC1} ${SRC2}
OBJ  = ${OBJ1} ${OBJ2}
HDR  = runner.h proc.h
TRG1 = truns
TRG2 = tcolr

${TRG1}: ${OBJ1}
	gcc -pthread ${OBJ1} -o ${TRG1}

${TRG2}: ${OBJ2}
	gcc -pthread ${OBJ2} -o ${TRG2}

main1.o: main1.c ${HDR}
	gcc -pthread -c main1.c

main2.o: main2.c ${HDR}
	gcc -pthread -c main2.c

error.o: error.c ${HDR}
	gcc -pthread -c error.c

runner.o: runner.c ${HDR}
	gcc -pthread -c runner.c

color.o: color.c ${HDR}
	gcc -pthread -g -c color.c

util.o: util.c ${HDR}
	gcc -pthread -c util.c

test1: ${TRG1}
	./${TRG1} 15 20000
	awk -f stats.awk pthread_stats

test2: ${TRG2}
	./${TRG2} 40 100 60
	awk -f stats.awk pthread_stats

clean:
	/bin/rm -f ${OBJ} ${TRG1} ${TRG2} pthread_stats

tar:
	tar cf ThreadPerf.tar ${SRC} ${HDR} makefile stats.awk

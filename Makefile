#Makefile for concurrency program in C lang
# libs https://www.cprogramming.com/tutorial/shared-libraries-linux-gcc.html

EXECUTABLE=concur
LIBDIR=${HOME}/libs
COMMONDIR=${HOME}/g/utilz_c
COMMONFLAGS= -Wall  -g -ggdb
all:  app

app:
	echo "... building app ${EXECUTABLE}..."
	gcc -o ${EXECUTABLE} ${COMMONFLAGS}  -I${COMMONDIR} -I./ multithread.c -lpthread -lutilz


clean:
	rm -rf *.o ${EXECUTABLE} ${LIBDIR}/libutilz.so
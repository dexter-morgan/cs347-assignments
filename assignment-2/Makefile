all: sched


sched: fcfs.o sjf.o stcf.o lottery.o util.o main.o
	gcc -g -o sched util.o fcfs.o sjf.o stcf.o lottery.o main.o

util.o: util.c sched.h
	gcc -g -c util.c -o util.o

main.o: main.c sched.h
	gcc -g -c main.c -o main.o

fcfs.o: fcfs.c sched.h
	gcc -g -c fcfs.c -o fcfs.o

sjf.o: sjf.c sched.h
	gcc -g -c sjf.c -o sjf.o

stcf.o: stcf.c sched.h
	gcc -g -c stcf.c -o stcf.o

lottery.o: lottery.c sched.h
	gcc -g -c lottery.c -o lottery.o

clean:
	rm sched *.o

all: CSUBatch Job1 Job2 Job3 Job4 Job5

CSUBatch: main.o
	gcc -o CSUBatch main.o -lpthread -pthread

main.o: main.c
	gcc -c main.c -pthread -lpthread

Job1: Job1.o
	gcc -o Job1 Job1.o 

Job2: Job2.o
	gcc -o Job2 Job2.o 

Job3: Job3.o
	gcc -o Job3 Job3.o 

Job4: Job4.o
	gcc -o Job4 Job4.o 

Job5: Job5.o
	gcc -o Job5 Job5.o -lm

Job1.o: ./Jobs/Job1.c
	gcc -c ./Jobs/Job1.c

Job2.o: ./Jobs/Job2.c
	gcc -c ./Jobs/Job2.c 

Job3.o: ./Jobs/Job3.c
	gcc -c ./Jobs/Job3.c 

Job4.o: ./Jobs/Job4.c
	gcc -c ./Jobs/Job4.c 

Job5.o: ./Jobs/Job5.c
	gcc -c ./Jobs/Job5.c -lm



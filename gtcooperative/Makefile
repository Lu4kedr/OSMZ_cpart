CFLAGS = -g -Wall

all: gttest
dummy: all
gttest: gthr.o gtswtch.o main.o
	$(CC) -o $@ $^

.S.o:
	as -o $@ $^

.PHONY: clean
clean:
	rm -f *.o gttest

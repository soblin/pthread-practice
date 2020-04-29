TARGET := main

CFLAGS := $(CFLAGS)
LDFLAGS := $(LDFLAGS) -pthread -lm

all: main.c
	$(CC) -o main main.c $(LDFLAGS)

clean:
	rm -rf $(TARGET) *.o

run: main
	./main

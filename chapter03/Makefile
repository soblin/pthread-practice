TARGET := main

CFLAGS := $(CFLAGS) -pthread
LDFLAGS := $(LDFLAGS) -pthread

all: $(TARGET)

firstThrerad: main.o
	$(CC) ($LDFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET) *.o fdProcess.txt fdThread.txt

run: main
	./main

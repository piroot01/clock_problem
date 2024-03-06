CC = g++ 
CFLAGS = -g -Wall -O3 --std=c++17

TARGET = main

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp

clean:
	$(RM) $(TARGET)

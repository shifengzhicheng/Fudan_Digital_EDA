# Compiler and linker settings
CC = g++
CFLAGS = -Wall -g
LDFLAGS =

# Source files and object files
SRCDIR = src
SRCS = $(wildcard $(SRCDIR)/*.cpp)
HDRS = $(wildcard $(SRCDIR)/*.h)
OBJS = $(SRCS:.cpp=.o)
TARGET = hls

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
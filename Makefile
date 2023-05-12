# 编译器
CC=g++
# 链接器
LD=g++
# 编译选项
CFLAGS=-c -g -DDEBUG -W -Wall -fPIC
# 链接选项
LDFLAGS=
# 可执行文件名称
EXECUTABLE=hls

# 所有的源文件
SOURCES=main.cpp parser.cpp HLS.cpp dataflowgraph.cpp controlflowgraph.cpp schedule.cpp Hungarian_algorithm.h control_logic.h cycleTable.h FSMachine.cpp

# 所有的对象文件
OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

.cpp.o:
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS)

.PHONY: clean
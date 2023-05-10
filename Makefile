LINK    = @echo linking $@ && g++ 
GCC     = @echo compiling $@ && g++ 
GC      = @echo compiling $@ && gcc 
AR      = @echo generating static library $@ && ar crv
FLAGS   = -g -DDEBUG -W -Wall -fPIC
GCCFLAGS = 
DEFINES = 
HEADER  = -I./
LIBS    = 
LINKFLAGS =

#LIBS    += -lrt
#LIBS    += -pthread



hls: parser.o main.o HLS.o
	$(LINK) $(FLAGS) $(LINKFLAGS) -o $@ $^ $(LIBS)

.cpp.o:
	$(GCC) -c $(HEADER) $(FLAGS) $(GCCFLAGS) -fpermissive -o $@ $<

.c.o:
	$(GC) -c $(HEADER) $(FLAGS) -fpermissive -o $@ $<

clean:
	rm -rf $(TARGET) *.o *.so *.a
	rm -rf hls

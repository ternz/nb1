
OBJ=main.o error.o helper.o multiplexer.o fdhandle.o packet.o ptrbuf.o processor.o io_processor.o proxy_handler.o
FLAG=-g
LIBS=-L.

all: mq-proxy2

mq-proxy2: $(OBJ)
	g++ $(FLAG) $(OBJ) -o $@  $(LIBS) -lpthread

%.o: %.cpp
	g++ $(FLAG) -c $< -o $@
	
clean:
	rm -f *.o mq-proxy2
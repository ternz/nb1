
OBJ=main.o error.o helper.o multiplexper.o fdhandle.o packet.o processor.o proxy_handler.o
FLAG=-D DEBUG -g
LIBS=-L.

all: mq-proxy

mq-proxy: $(OBJ)
	g++ $(FLAG) $(OBJ) -o $@  $(LIBS) -lpthread

%.o: %.cpp
	g++ $(FLAG) -c $< -o $@
	
clean:
	rm -f *.o mq-proxy
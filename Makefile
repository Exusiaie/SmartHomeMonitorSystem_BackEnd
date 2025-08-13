SRCS:=$(wildcard *.cc)
OBJS:=$(patsubst %.cc,%.o,$(SRCS))
BIN:=TestEchoServer.exe
CC:=g++
FLAGS:=-g -w

$(BIN):$(OBJS)
	$(CC) -o $@ $^ $(FLAGS)
%.o:%.cc
	$(CC) -c $^ -o $@ $(FLAGS)

clean:
	rm -rf $(OBJS) $(BIN)

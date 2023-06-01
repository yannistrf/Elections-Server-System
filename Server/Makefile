# our compiler
CC = g++

# where to look for header files
INCLUDE = ./include

# compiler flags
CFLAGS = -I$(INCLUDE) -Wall -g #-Werror

# linker flags
LDFLAGS = -g -lpthread

ARGS = 5050 4 16 poll-log poll-stats

# executable name
EXEC = poller

# binary folder
BIN = ./bin

# source code folder
SRC = ./src

# all of our object files
OBJS = 	$(BIN)/poller.o $(BIN)/sig.o $(BIN)/netServer.o \
		$(BIN)/threadsServer.o $(BIN)/socket_buffer.o $(BIN)/poll.o\

# rule to build executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

# rule to build our object files
$(BIN)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

# run executable
run: $(EXEC)
	./$(EXEC) $(ARGS)

# clean all binary files
clean:
	rm -f $(OBJS) $(EXEC)

val: $(EXEC)
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXEC) $(ARGS)
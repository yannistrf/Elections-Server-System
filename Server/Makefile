# our compiler
CC = g++

# where to look for header files
INCLUDE = ./include

# compiler flags
CFLAGS = -I$(INCLUDE) -Wall -g #-Werror

# linker flags
LDFLAGS = -g

ARGS = 5050 3 16 poll-log poll-stats

# executable name
EXEC = poller

# binary folder
BIN = ./bin

# source code folder
SRC = ./src

# all of our object files
OBJS = 	$(BIN)/poller.o $(BIN)/net.o $(BIN)/util_ser.o \
		$(BIN)/threads.o $(BIN)/socket_buffer.o $(BIN)/poll.o \
		$(BIN)/sig_exit.o

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
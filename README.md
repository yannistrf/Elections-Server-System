# Elections-Server-System

## Ioannis Triantafyllopoulos sdi2000193

## Build
When in the root of the project run "make". This will build
the executables of the server and the client. Open two
different terminals, navigate to the Server/Client directories
and run "make run". Feel free to mess with the arguments
inside the Makefiles of each directory. To delete all binaries,
go to the root of the project and run "make clean".

## Client
The Client part of the project is fairly simple. It's main
purpose is to simulate multiple clients that communicate with
the server at the same time. By providing the server name and
a file with votes in it, there will be created a thread for
every vote and try to send the vote to the server.

## Communication
The protocol we use for the network communication is pretty
straight forward. Whenever one of the 2 sides wants to send a
message to the other side, it needs to send first an integer
that will tell him the size of the incoming message. So if we
want to send the message "hello world", we will first send the
int 11 and after that the actual string.

## Server
For the server implementation things are a little more complicated.
First of all, our server is multithreaded. We have the master thread
which accepts incoming connections and puts the socket descriptor
inside a buffer and the worker threads that perform the communication
with the clients. The buffer is actually a queue so that we have the
fair, in this situation, FIFO policy.

Each worker thread must take the vote from each client and update the
current election data. To accomplish that, we have a set which holds
the unique names of the voters, a map that holds the number of votes
for each party, and a file where we record every vote.

### Synchronisation
All of the threads (including master) will have access to the buffer
which means we will need to handle the race conditions that occur.
For that we use a condition variable provided by the pthread's API.
Basically, if the buffer is empty the worker threads go to sleep until
a new connection has been put in the buffer. Then one of the threads
will wake up and serve the client. In that way, we avoid busy waiting
and any incosistent states, since in order to access the buffer the
mutex must be acquired. (socket_buffer.cpp/hpp)

We need to mention that the buffer has a max size, which means that if
it's full, then the master thread must wait until the buffer has an
empty spot. For that we use another condition variable which puts the
main thread to sleep and wakes up when a worker thread pops a socket
descriptor from the queue. (socket_buffer.cpp/hpp)

All of the worker threads will also have access to a set, to a map and
to a file. To synchronise the access to those 3 we just use a mutex.
Whenever a worker thread wants to read or write to them it has to
acquire the mutex and when it's finished to release it. (poll.cpp/hpp)

### Signals
We want for the master thread to always catch SIGINT and terminate properly
the program. For that we have a global variable which indicates if the
server is accepting incoming connections. So before accepting a new connection
we check this variable and proceed as we should. To avoid another thread
catching the signal, we make all of the worker threads to block SIGINT.

We also handle the SIGPIPE signal in case the communication is stopped
unexpectedly. Whenever there is an error of this kind, we just close
the sockets and continue waiting for incoming connections.
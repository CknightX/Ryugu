CC = g++ -std=c++11 -g -pthread
OBJ = channel.o EventHandler.o poller.o conn.o utils.o TcpServer.o net.o buffer.o\
TimerQueue.o Timestamp.o Timer.o EventHandlerThread.o 

all : $(OBJ) test.o 
	$(CC) $(OBJ) test.o -o test

buffer.o : buffer.cpp
	$(CC) $< -c -o $@

test.o : test.cpp
	$(CC) $< -c -o $@

channel.o : channel.cpp
	$(CC) $< -c -o $@

EventHandler.o : EventHandler.cpp
	$(CC) $< -c -o $@

poller.o : poller.cpp
	$(CC) $< -c -o $@

conn.o : conn.cpp
	$(CC) $< -c -o $@

utils.o : utils.cpp
	$(CC) $< -c -o $@

TcpServer.o : TcpServer.cpp
	$(CC) $< -c -o $@

net.o : net.cpp
	$(CC) $< -c -o $@
Timer.o : Timer.cpp
	$(CC) $< -c -o $@
Timestamp.o : Timestamp.cpp
	$(CC) $< -c -o $@
TimerQueue.o : TimerQueue.cpp
	$(CC) $< -c -o $@

EventHandlerThread.o : EventHandlerThread.cpp
	$(CC) $< -c -o $@




clean:
	rm -f *.o
	rm -f test

run :
	./test
CC = g++ -std=c++11 -g -pthread
OBJ = Channel.o EventHandler.o Poller.o TcpConn.o Utils.o TcpServer.o Net.o Buffer.o\
TimerQueue.o Timestamp.o Timer.o EventHandlerThread.o EventHandlerThreadPool.o 

all : $(OBJ) Test.o 
	$(CC) $(OBJ) Test.o -o Test

Buffer.o : Buffer.cpp
	$(CC) $< -c -o $@

Test.o : Test.cpp
	$(CC) $< -c -o $@

Channel.o : Channel.cpp
	$(CC) $< -c -o $@

EventHandler.o : EventHandler.cpp
	$(CC) $< -c -o $@

Poller.o : Poller.cpp
	$(CC) $< -c -o $@

TcpConn.o : TcpConn.cpp
	$(CC) $< -c -o $@

Utils.o : Utils.cpp
	$(CC) $< -c -o $@

TcpServer.o : TcpServer.cpp
	$(CC) $< -c -o $@

Net.o : Net.cpp
	$(CC) $< -c -o $@
Timer.o : Timer.cpp
	$(CC) $< -c -o $@
Timestamp.o : Timestamp.cpp
	$(CC) $< -c -o $@
TimerQueue.o : TimerQueue.cpp
	$(CC) $< -c -o $@
EventHandlerThread.o : EventHandlerThread.cpp
	$(CC) $< -c -o $@
EventHandlerThreadPool.o : EventHandlerThreadPool.cpp
	$(CC) $< -c -o $@




clean:
	rm -f *.o
	rm -f test

run :
	./test
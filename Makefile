CC = g++ -std=c++11 -g
OBJ = echo.o channel.o eventhandler.o poller.o conn.o utils.o tcpserver.o net.o buffer.o\
TimerQueue.o Timestamp.o Timer.o


all : $(OBJ)
	$(CC) $(OBJ) -o echo 

test : test.cpp buffer.o
	$(CC) $< buffer.o -o $@

buffer.o : buffer.cpp
	$(CC) $< -c -o $@

echo.o : echo.cpp
	$(CC) $< -c -o $@

channel.o : channel.cpp
	$(CC) $< -c -o $@

eventhandler.o : eventhandler.cpp
	$(CC) $< -c -o $@

poller.o : poller.cpp
	$(CC) $< -c -o $@

conn.o : conn.cpp
	$(CC) $< -c -o $@

utils.o : utils.cpp
	$(CC) $< -c -o $@

tcpserver.o : tcpserver.cpp
	$(CC) $< -c -o $@

net.o : net.cpp
	$(CC) $< -c -o $@
Timer.o : Timer.cpp
	$(CC) $< -c -o $@
Timestamp.o : Timestamp.cpp
	$(CC) $< -c -o $@
TimerQueue.o : TimerQueue.cpp
	$(CC) $< -c -o $@




clean:
	rm -f *.o
	rm -f test echo

run :
	./test
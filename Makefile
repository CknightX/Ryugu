CC = g++ -std=c++14 -g -pthread

SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SOURCES))
 
TARGET  = Test
 
all:$(OBJS)
	#$(CC) -o $(TARGET) $(OBJS)	
	$(CC) -o server $(OBJS)	WebServer/Server_test.cpp

lib:$(OBJS)
	ar -rc libryugu.a $(OBJS)

%.o:%.c
	$(CC) -c $(CFLAGS_WARN) $(DEFINE) $(INCLUDE) $< -o $@	
clean:	
	rm -f $(OBJS)
	rm -f $(TARGET)

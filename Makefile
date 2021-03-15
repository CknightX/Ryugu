CC = g++ -std=c++11 -g -pthread

SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SOURCES))
 
TARGET  = Test
 
all:$(OBJS)
	$(CC) -o $(TARGET) $(OBJS)	
 
%.o:%.c
	$(CC) -c $(CFLAGS_WARN) $(DEFINE) $(INCLUDE) $< -o $@	
clean:	
	rm -f $(OBJS)
	rm -f $(TARGET)

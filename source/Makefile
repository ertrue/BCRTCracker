CXXFLAGS = -g3 
CCFLAGS = -g3 

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: CCFLAGS += -DDEBUG -g
debug: executable

executable: main.o components.o
	$(CXX) $(CXXFLAGS) -o bcrtcraker main.o components.o -lstdc++ 

main.o: main.cpp components.h
	$(CC) $(CCFLAGS) -c main.cpp

components.o: components.cpp components.h
	$(CC) $(CCFLAGS) -c components.cpp

clean:
	rm -rf main.o components.o bcrtcraker

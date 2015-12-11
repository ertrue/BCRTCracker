include ./make.include

CXXFLAGS += -g3 
CCFLAGS += -g3 

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: CCFLAGS += -DDEBUG -g
debug: executable

executable: main.o components.o
	$(CXX) $(CXXFLAGS) -o bcrtcracker main.o components.o -lstdc++ 

sim: BCRTForm.o gamer.o
	$(CXX) $(CXXFLAGS) $(INC) -o bcrtsim gamer.o BCRTForm.o $(LIBS)

main.o: main.cpp components.h
	$(CC) $(CCFLAGS) -c main.cpp

components.o: components.cpp components.h
	$(CC) $(CCFLAGS) -c components.cpp

BCRTForm.o: BCRTForm.cpp BCRTForm.h
	$(CXX) $(CXXFLAGS) -c BCRTForm.cpp $(INC) 

gamer.o: gamer.cpp BCRTForm.h
	$(CXX) $(CXXFLAGS) -c gamer.cpp  $(INC)

clean:
	rm -rf main.o components.o bcrtcracker bcrtsim

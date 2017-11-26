
#makefile for raspberry and robocup jr contest

#CXXFLAGS = -D _DEBUG_SERIAL_

#elenco dei file in C

c_src = $(wildcard src/C/*.c)

#elenco dei file in c++

ccsrc = $(wildcard src/*.cpp) 
#$(wildcard src/*.cpp) 
#        $(wildcard src/mot/*.cc) \
#        $(wildcard src/audio/*.cc)

# per ogni nome in ccsrc, sostituire il suffisso .cpp con .o
obj = $(ccsrc:.cpp=.o) 

# al momento nn serve  la cartella coi file in C
obj += $(c_src:.c=.o)


#librerie da collegare

#LDFLAGS = -lwiringPi -lm
LDFLAGS = 

testProtUart: $(obj)
	$(CXX)  -o $@ $(obj) $(LDFLAGS) 



.PHONY: clean
clean:
	rm -f $(obj) testProtUart

pippo:
	$(obj)

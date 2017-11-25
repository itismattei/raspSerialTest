
#makefile for raspberry and robocup jr contest

#elenco dei file in C

c_src = $(wildcard src/C/*.c)

#elenco dei file in c++

ccsrc = $(wildcard *.c) 
#$(wildcard src/*.cpp) 
#        $(wildcard src/mot/*.cc) \
#        $(wildcard src/audio/*.cc)

# per ogni nome in ccsrc, sostituire il suffisso .cpp con .o
obj = $(ccsrc:.c=.o) 

# al momento nn serve  la cartella coi file in C
#obj += $(c_src:.c=.o)


#librerie da collegare

LDFLAGS = #-lwiringPi -lm

test_prog: $(obj)
	$(CC) -o $@ $(obj) $(LDFLAGS) 



.PHONY: clean
clean:
	rm -f $(obj) test_prog

pippo:
	$(obj)

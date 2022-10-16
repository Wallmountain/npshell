++ = g++

obj:=npshell.o command.o pipe.o process.o build-in_command.o
exe:=npshell
CFLAGS=-O1 -g -Wall

all:$(obj)
	$(++) -o $(exe) $(obj)

%.o:%.c
	$(++) -c $^ $(CFLAGS) -o $@

clean:
	rm -rf $(exe) $(obj)
.PHONY:all
cc = g++
all:server client 
server:Tcp_Server.c 
	$(cc) -o $@ $^ -g
client:Tcp_Client.cc
	$(cc) -o $@ $^ 
.PHONY:clean
clean:
	rm -rf server client


all: server connect

server: HeRcestServer.c
	gcc HeRcestServer.c -o serverHRC -Wall -lpthread

connect: HeRcestClient.c
	gcc HeRcestClient.c -o connect -Wall -lpthread
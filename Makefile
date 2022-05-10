client: client_exe
	./client_exe

server: server_exe
	./server_exe

clean:
	rm client_exe
	rm server_exe

client_exe: client.cpp
	g++ -o client_exe client.cpp register.pb.cc -lprotobuf
	
server_exe: server.cpp
	g++ -o server_exe server.cpp register.pb.cc -lprotobuf
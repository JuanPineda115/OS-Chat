client: client_exe
	./client_exe 127.0.0.1 54000

server: server_exe
	./server_exe 54000

clean:
	rm client_exe
	rm server_exe

client_exe: client.cpp
	g++ -o client_exe client.cpp register.pb.cc -lprotobuf -lpthread 
	
server_exe: server.cpp
	g++ -o server_exe server.cpp register.pb.cc -lprotobuf --std=c++0x -lpthread 
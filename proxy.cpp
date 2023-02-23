#include<sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <sys/time.h>
#include <sys/types.h>
#include <thread>

#include "netdb.h"
#include "TCPClient.hpp"
#include "TCPServer.hpp"
#include "ClientRequest.hpp"
#include "ServerResponse.hpp"
#include "SocketUtils.hpp"
#include "Cache.hpp"
// #include "messageHandle.hpp"

class Proxy{
private:
    std::string host;
    std::string port;

    Cache c;
public:
    Proxy(std::string p):host(NULL), port(p){}

    void run(){
        int socket_fd=server_init(port);
		int req_id=0;
		while(true){
			struct sockaddr_storage their_addr;
			socklen_t addr_size = sizeof(their_addr);
			int client_fd=accept(socket_fd, (struct sockaddr *)&their_addr, &addr_size);
			std::string client_IP=inet_ntoa(((struct sockaddr_in*)&their_addr)->sin_addr);
			
			std::thread t(&Proxy::request_process, this, client_fd, req_id, client_IP);
			t.detach();
			//  pthread_create(&thread,NULL,&request_process,fds);
			
			req_id+=1;
		}
	}
	void request_process(int client_fd, int req_id, std::string client_IP ){

		//recieve request
		char msg[2147483647];//maximum 65536
		int length=recv(client_fd,msg,sizeof(msg),0);
		//need error handle

		std::string request_info=std::string(msg,length);
		 //pthread_mutex_lock(&lock);
		// ClientRequest* request=new ClientRequest(request_info,req_id,((std::string*)fds)[2]);
		//ClientRequest* request=new ClientRequest(request_info,req_id,client_IP);
		
		http::request<http::string_body> request = parseRequest(request_info);

		
		http::verb method = request.method();
        std::string host=std::string(request.at("HOST"));
        std::string port=request->port;

        int server_fd=client_init(host,port);

		bool is_bad_request=(method!="GET")&&(method!="POST")&&(method!="CONNECT");
		if(is_bad_request){
			std::cerr<<"Bad Request Type!!!"<<std::endl;
			exit(1);
		}
	    if(method=="POST"){
	    	post(length,request_info,request,server_fd,client_fd);
		}else if(method=="GET"){
			get(length,request_info,request,server_fd,client_fd);
		}else if(method=="CONNECT"){
			connect(length,request_info,request,server_fd,client_fd);
		}
		 //pthread_mutex_unlock(&lock);
	}
	ServerResponse* forward(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd){
		int status_1=send(server_fd,request_info.c_str(),length,0);
		if(status_1==-1){
			std::cerr<<"send error!!"<<std::endl;
		}
		char response_char[2147483647];
		int len=recv(server_fd,response_char,sizeof(response_char),0);
		std::string Response_Info=std::string(response_char,len);
		ServerResponse* response=new ServerResponse(Response_Info);
		int status_2= send(client_fd,Response_Info.c_str(),len,0);
		if(status_2==-1){
			std::cerr<<"send error!!!"<<std::endl;
		}
		return response;
	}

	void post(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd){
		ServerResponse* response=forward(length,request_info,request,server_fd,client_fd);
	}
	void get(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd){
		ServerResponse* response=forward(length,request_info,request,server_fd,client_fd);
	}
	void connect(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd){
		struct timeval time;
		time.tv_sec = 2;
		time.tv_usec = 0;
		
		fd_set master;
		fd_set readfds;
		FD_ZERO(&master);
		int nfds = server_fd > client_fd ? server_fd+1 : client_fd+1;
		int fd_list[] = {server_fd, client_fd};
		for(int i = 0;i < 2; i++){
			FD_SET(fd_list[i], &master);
		}
		int status;
		//listen to incoming patatos and end
		while(true){
			fd_set readfds = master;
			//recieve potato
			status = select(nfds, &readfds, NULL, NULL, NULL);
		
			if(status == 0){
				std::cerr<<"listen time limit"<<std::endl;
				break;
			}
			else if(status == 1){
				int socket;
				if(FD_ISSET(server_fd, &readfds)){
					socket = server_fd;
				}else{
					socket = client_fd;
				}

			}

			ServerResponse* response=forward(length,request_info,request,server_fd,client_fd);
		}
	}

};

// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// void*request_process(void* fds);
// void post(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd);
// void get(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd);
// void connect(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd);



int main(){
	Proxy p("12345");
	p.run();
	return 0;
}

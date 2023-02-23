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
// #include "Cache.hpp"

class Proxy{
private:
    std::string port;

    // Cache c;
public:
    Proxy(std::string p):port(p){}

    void run(){
        int socket_fd=server_init(port);
		int req_id=0;
		while(true){
			struct sockaddr_storage their_addr;
			socklen_t addr_size = sizeof(their_addr);
			int client_fd=accept(socket_fd, (struct sockaddr *)&their_addr, &addr_size);
			std::string client_IP=inet_ntoa(((struct sockaddr_in*)&their_addr)->sin_addr);
			std::cout<<"try1"<<std::endl;
			std::thread t(&Proxy::request_process, this, client_fd, req_id, &client_IP);

			t.detach();
			//  pthread_create(&thread,NULL,&request_process,fds);
			
			req_id+=1;
		}
	}

	bool isHTTPS(std::string hostname, std::string * host, std::string * port){
        size_t f = hostname.find(":");
        if(f != std::string::npos){
            *host = hostname.substr(0, f);
            *port = hostname.substr(f+1);
            return true;
        }
        *host = hostname;
        *port = "80";
        return false;
    }
	void request_process(int client_fd, int req_id, std::string * client_IP ){
		// std::cout<<"try1.5"<<std::endl;
		//recieve request
		char msg[65536];//maximum 65536
		int length=recv(client_fd,msg,sizeof(msg),0);
		//need error handle
		// std::cout<<"try2"<<std::endl;

		std::string request_info=std::string(msg,length);

		http::request<http::string_body> request = parseRequest(request_info);
		std::cout<<request <<std::endl;
		std::string host;
		std::string port;
		http::verb method = request.method();
		isHTTPS(std::string(request.at("HOST")), &host, &port);
		// std::cout<<"try3"<<std::endl;
        int server_fd=client_init(host,port);

		if (method ==http::verb::get){
            get(length,&request_info,&request,server_fd,client_fd);
		}
        else if(method == http::verb::post){
            post(length,&request_info,&request,server_fd,client_fd);

        }else if(method == http::verb::connect){
            // connect(length,request_info,&request,server_fd,client_fd);
        }else{
            std::cerr<<"Bad Request Type!!!"<<std::endl;
			exit(1);
        }
	}

	void forward(int length,std::string * request_info, http::request<http::string_body> * request,int server_fd, int client_fd){
		int status_1=send(server_fd,request_info->c_str(),length,0);
		if(status_1==-1){
			std::cerr<<"send error!!"<<std::endl;
		}
		char response_char[65536]; //65536 maximum.
		int len=recv(server_fd,response_char,sizeof(response_char),0);
		std::string response_Info=std::string(response_char,len);

		http::response<http::string_body> response = parseResponse(response_Info);

		// ServerResponse* response=new ServerResponse(Response_Info);
		int status_2= send(client_fd,response_Info.c_str(),len,0);
		if(status_2==-1){
			std::cerr<<"send error!!!"<<std::endl;
		}
	}

	void post(int length,std::string * request_info, http::request<http::string_body> * request,int server_fd, int client_fd){
		forward(length,request_info,request,server_fd,client_fd);
		// ServerResponse* response=forward(length,request_info,request,server_fd,client_fd);
	}
	void get(int length,std::string * request_info,http::request<http::string_body> * request,int server_fd, int client_fd){
		forward(length,request_info,request,server_fd,client_fd);
	}
	void connect(int length,std::string * request_info,http::request<http::string_body> * request,int server_fd, int client_fd){
		// struct timeval time;
		// time.tv_sec = 2;
		// time.tv_usec = 0;
		
		fd_set master;
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
				if(FD_ISSET(server_fd, &readfds)){
					forward(length,request_info,request,server_fd,client_fd);
				}else{
					char msg[65536];//maximum 65536
					int length2=recv(client_fd,msg,sizeof(msg),0);
					//need error handle

					std::string request_info2=std::string(msg,length);

					http::request<http::string_body> request2 = parseRequest(request_info2);
					forward(length,&request_info2,&request2,server_fd,client_fd);
				}
			}
			// ServerResponse* response=forward(length,request_info,request,server_fd,client_fd);
		}
	}

};

int main(){
	Proxy p("12345");
	p.run();
	return 0;
}

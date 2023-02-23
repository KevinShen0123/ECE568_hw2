#include<sys/socket.h>
#include "netdb.h"
#include <unistd.h>
#include <iostream>
#include "TCPClient.hpp"
#include "TCPServer.hpp"
#include "ClientRequest.hpp"
#include "ServerResponse.hpp"
#include <pthread.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include<fstream>
#include "Cache.hpp" 
#include<time.h>
#include<ctime>
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
Cache* response_cache=new Cache();
std::ofstream LogStream("proxy.log");
void*request_process(void* fds);
void post(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd);
void get(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd);
void connect(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd);
void start(){
	int socket_fd=server_init(std::string("12345"));
	int req_id=0;
	while(1){
		 struct sockaddr_storage their_addr;
	     socklen_t addr_size;
	     int client_fd=accept(socket_fd, (struct sockaddr *)&their_addr, &addr_size);
	     std::string client_IP=inet_ntoa(((struct sockaddr_in*)&their_addr)->sin_addr);
	   	 pthread_t thread;
	   	 std::stringstream ss1;
         ss1 << client_fd;
         std::string client_fd_str = ss1.str();
         std::stringstream ss2;
          ss2 << req_id;
         std::string req_id_str = ss2.str();
         pthread_mutex_lock(&lock);
	   	 std::string*fds=new std::string[3];
	   	 fds[0]=client_fd_str; 
	   	 fds[1]=req_id_str;
	   	 fds[2]=client_IP;
	   	 pthread_mutex_unlock(&lock);
	   	 pthread_create(&thread,NULL,&request_process,fds);
	   	 req_id+=1;
	}
}
void* request_process(void* fds){
	    std::string client_fd_str=((std::string*)fds)[0];
	    int client_fd=atoi(client_fd_str.c_str());
	    std::string req_id_str=((std::string*)fds)[1];
	    int req_id=atoi(req_id_str.c_str());
		std::vector<char> msg(2147483647,0);
		int length=recv(client_fd,&(msg.data()[0]),2147483647,0);
		std::string request_info=std::string(msg.data(),length);
		 //pthread_mutex_lock(&lock);
		ClientRequest* request=new ClientRequest(request_info,req_id,((std::string*)fds)[2]);
		pthread_mutex_lock(&lock);
		std::time_t now=std::time(0);
		std::string currentTime=std::asctime(std::localtime(&(now)));
		LogStream<<request->ID<<": '" <<request->line_one<<"'"<<"from"<<request->IP<<"@"<<currentTime<<std::endl;
		pthread_mutex_unlock(&lock);
		std::string method=request->request_method;
        std::string host=request->host_name;
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
			ServerResponse* tryGetResponse=response_cache->check_request_save(LogStream,lock,server_fd,request);
			if(tryGetResponse==NULL){
				get(length,request_info,request,server_fd,client_fd);
			}else{
				int sstatus=send(client_fd,tryGetResponse->whole_response.c_str(),tryGetResponse->whole_response.length()+1,0);
				if(sstatus==-1){
					std::cerr<<"send error!!!"<<std::endl;
				}
			}
		}else if(method=="CONNECT"){
			connect(length,request_info,request,server_fd,client_fd);
		}
		 //pthread_mutex_unlock(&lock);
	close(server_fd);
	close(client_fd);
   return NULL;		
}

ServerResponse* forward(bool need_cache,int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd){
	int status_1=send(server_fd,request_info.c_str(),length,0);
  if(status_1==-1){
  	std::cerr<<"send error!!"<<std::endl;
  }
  std::vector<char> response_char(2147483647,0);
  int len=recv(server_fd,&(response_char.data()[0]),2147483647,0);
  std::string Response_Info=std::string(response_char.data(),len);
  ServerResponse* response=new ServerResponse(Response_Info);
  if(need_cache&&response->is_200_ok){
  	 response_cache->check_response_save(LogStream,lock,server_fd,request,response);
  }
  int status_2= send(client_fd,Response_Info.c_str(),len,0);
  if(status_2==-1){
  	std::cerr<<"send error!!!"<<std::endl;
  }
  return response;
}
void post(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd){
  ServerResponse* response=forward(false,length,request_info,request,server_fd,client_fd);
}
void get(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd){
   ServerResponse* response=forward(true,length,request_info,request,server_fd,client_fd);
}
void connect(int length,std::string request_info,ClientRequest* request,int server_fd, int client_fd){
	std::string connect_response=std::string("HTTP/1.1 200 OK\r\n\r\n");
	send(server_fd,connect_response.c_str(),connect_response.length()+1,0);
	int max_fd=server_fd;
	if(client_fd>=server_fd){
		max_fd=client_fd;
	}
	fd_set fdsReader;
	std::vector<int> fds;
	fds.push_back(server_fd);
	fds.push_back(client_fd);
	FD_SET(server_fd,&fdsReader);
	FD_SET(client_fd,&fdsReader);
	while(true){
		if(select(max_fd+1,&fdsReader,NULL,NULL,NULL)<0){
			FD_ZERO(&fdsReader);
			FD_SET(server_fd,&fdsReader);
	        FD_SET(client_fd,&fdsReader);
		}else if(select(max_fd+1,&fdsReader,NULL,NULL,NULL)==0){
			std::cerr<<"Time out!!"<<std::endl;
		}else{
			int recver=client_fd;
			int sender=server_fd;
			if(FD_ISSET(fds[0],&fdsReader)){
				recver=fds[0];
				sender=fds[1];
			}
			std::vector<char> r1(2147483647,0);
		    int status1=recv(recver,&(r1.data()[0]),sizeof(r1),0);
		    if(status1<=0){
		    	break;
			}
			std::string r1info=std::string(r1.data(),status1);
			int status2=send(sender,r1info.c_str(),r1info.length()+1,0);
			if(status2<=0){
				break;
			}
			if(FD_ISSET(fds[1],&fdsReader)){
				recver=fds[1];
				sender=fds[0];
			}
			std::vector<char>r2(2147483647,0);
		    int status3=recv(recver,&(r2.data()[0]),2147483647,0);
		    if(status3<=0){
		    	break;
			}
			std::string r2info=std::string(r2.data(),status3);
			int status4=send(sender,r2info.c_str(),r2info.length()+1,0);
			if(status4<=0){
				break;
			}
		}
	}
}
int main(){
	start();
	return 0;
}

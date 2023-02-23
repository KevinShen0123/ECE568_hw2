#include <iostream>
#include "ServerResponse.hpp"
#include "ClientRequest.hpp"
#include <vector>
class Cache{
    std::map<std::string, std::string> cache_map;
	Cache(){
	    
	}
	bool in_cache(std::string request);
	bool validate(int server_fd, ClientRequest* request,ServerResponse* response);
	bool can_cache(std::string response);
	bool check_expire(int id,std::string info,ServerResponse* response);
	std::vector<std::string> get_cache_control(ServerResponse* response);
	ServerResponse* check_request_save(int server_fd,ClientRequest* request);
	void check_response_save(int server_fd,ClientRequest* request, ServerResponse* response);
}; 

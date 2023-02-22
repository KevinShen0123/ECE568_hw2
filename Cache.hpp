#include <iostream>
#include "ServerResponse.hpp"
class Cache{
    std::map<std::string, std::string> cache_map;
	Cache(){
	    
	}
	bool in_cache(std::string request);
	bool validate(std::string request,ServerResponse* response);
	bool can_cache(std::string response);
	bool check_expire(ServerResponse* response);
	std::vector<std::string> get_cache_control(ServerResponse* response);
	void   check_request_save(int req_id,std::string request_line);
	void check_response_save(int req_id,std::string request_line, std::string response_info);
}; 

<<<<<<< HEAD
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
=======
class Cache{
    std::map<std::string, std::string> cache_map;
    std::string request_line;
    std::string response_info;
	Cache(std::string request_line, std::string response_info){
		this->request_line=request_line;
		this->response_info=response_info;
	}
	bool in_cache(std::string request);
	bool validate(std::string request);
	std::string get_cache(std::string request);
	bool can_cache(std::string response);
	std::string  try_save();
>>>>>>> 13b42006e7c30764211d4d23628b1e21a11fb56d
}; 

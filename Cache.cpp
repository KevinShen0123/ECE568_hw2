#include "Cache.hpp"
#include <iostream>
<<<<<<< HEAD
#include "ServerResponse.hpp"
#include <ctime>
bool Cache::in_cache(std::string request){
	return this->cache_map.find(request)!=this.cache_map.end();
}
bool validate(std::string request, ServerResponse* response){
	//need implementation
	return true;
}
bool Cache::check_expire(std::string info,ServerResponse* response){
	std::<std::string, std::string> info_map=response->response_info;
	bool has_expire=(info_map.find("Expires")!=info_map.end());
    //if expired log info, return true, else, return false;
    return false;
}
std::vector<std::string> get_cache_control(ServerResponse* response){
		std::<std::string, std::string> info_map=response->response_info;
		bool has_cache_control=(info_map.find("Cache-Control")!=info_map.end());
		std::vector<std::string> cache_controls;
		std::string controls=info_map.find("Cache-Control")->second;
		char* cache_str;
		cache_str=strtok(controls.c_str(),",");
		while(cache_str!=NULL){
			cache_controls.push_back(std::string(cache_str));
			cache_str=strtok(NULL,",");
		}
	     return cache_controls;
}
void check_request_save(int req_id,std::string request_line){
	if(in_cache(request_line)==false){
		std::cout<<req_id<<":"<<" not in cache"<<std::endl;
		return;
	}
	std::string cached_response=cache_map.find(request_line)->second;
	ServerResponse* response=new ServerResponse(cached_response);
    bool has_expire=check_expire(req_id+": in cache, but expired at",response);
    std::vector<std::string> controls=get_cache_control(response);
    if(std::find(controls.begin(),controls.end(),"no-cache")!=controls.end()){
    	std::cout<<req_id+": in cache, required validation"<<std::endl;
        return;
	}
	if(validate(request_line,response)){
			std::cout<<req_id+": valid"<<std::endl;
        return;
	}
}
void check_response_save(int req_id,std::string request_line, std::string response_info){
	std::vector<std::string> controls=get_cache_control(new ServerResponse(response_info));
	if(in_cache(request_line==false)){
		bool no_store=(std::find(controls.begin(),controls.end(),"no-store")!=controls.end());
		if(no_store){
			std::cout<<"Not cacheable because no-store"<<std::endl;
			return;
		}
		this->cache_map.insert(this->cache_map.begin(),std::pair<std::string,std::string>(request_line,response_info);
		return;
	}
	std::string cached_response=this->cache_map.find(request_line)->second;
	ServerResponse* response=new ServerResponse(cached_response);
	bool has_expire=check_expire(req_id+": cached, expires at",response);
	std::vector<std::string> old_controls=get_cache_control(response);
	bool a=std::find(old_controls.begin(),old_controls.end(),"no-cache")!=old_controls.end();
	bool b=false;
	for(int i=0;i<old_controls.size();i++){
		std::string cache_control=controls[i];
		if(cache_control.find("revalidate")!=std::string::npos){
			b=true;
		}
	}
	if(a||b){
		std::cout<<req_id<<"cached, but requires revalidation"<<std::endl;
	}
	this->cache_map.insert(this->cache_map.begin(),std::pair<std::string, std::string>(request_line,response_info));
}
=======
bool Cache::in_cache(std::string request){
	return this->cache_map.find(request)!=this.cache_map.end();
}
bool Cache::validate(std::string request){
	return true;
}
std::string get_cache(std::string request){
	return "";
}
bool Cache::can_cache(std::string response){
	return true;
}
std::string Cache::trysave(){
	if(in_cache(this->request_line)&&validate(this->request_line)){
		return (this->cache_map.find(this->request_line))->second;
	}else if(in_cache(this->request_line)&&!validate(this->request_line)){
		return "ID: in cache, but expired at EXPIREDTIME";
	}else if(!in_cache(this->request_line)){
		this->cache_map.insert(new pair<std::string,std::string>(this->request_line,this->response_info));
		return "not in cache";
	}
}
>>>>>>> 13b42006e7c30764211d4d23628b1e21a11fb56d

#include <string>
class ServerResponse{
	bool is_200_ok;
	std::string http_version;
	std::string response_body;
	std::string status_info;
	std::map<std::string,std::string> response_info;
	public:
		ServerResponse(std::string response){
			parse(response);
		}
		void parse(std::string response_parse);
	   void parseHead(std::string head);
	   void parseMiddle(std::string middle);
	   void parseBody(std::string body);
}; 
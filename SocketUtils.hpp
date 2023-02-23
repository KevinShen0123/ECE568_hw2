#include <string>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace beast = boost::beast;         
namespace http = beast::http;           
namespace net = boost::asio;            
using tcp = boost::asio::ip::tcp;

http::request<http::string_body> parseRequest(std::string message){
    beast::error_code ec;
    http::request_parser<http::string_body> p;

    auto buf = boost::asio::buffer(message.data(), message.size());
    p.put(buf, ec);
    http::request<http::string_body> res = p.get();
    // *res = p.get();
    return res;
}

http::response<http::string_body> parseResponse(std::string message){
    beast::error_code ec;
    http::response_parser<http::string_body> p;

    auto buf = boost::asio::buffer(message.data(), message.size());
    p.put(buf, ec);
    http::response<http::string_body> res = p.get();
    // *res = p.get();
    return res;
}

int server_init(std::string port){
    int status;
    struct addrinfo server_info;
    struct addrinfo*server_info_list;

    memset(&server_info,0,sizeof(server_info));

    server_info.ai_family = AF_UNSPEC;
    server_info.ai_socktype = SOCK_STREAM;
    server_info.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(NULL, port.c_str(), &server_info, &server_info_list)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    int server_fd=socket(server_info_list->ai_family,server_info_list->ai_socktype,server_info_list->ai_protocol);
    int one;
    status=setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(int));
    if(status<0){
        perror("setting socket error!!");
        exit(1);
    }
    bind(server_fd,server_info_list->ai_addr,server_info_list->ai_addrlen);
    int listen_yes=listen(server_fd,200);
    if(listen_yes==-1){
        perror("listen error!!");
        exit(1);
    }
    freeaddrinfo(server_info_list);
    return server_fd;
}

int client_init(std::string host, std::string port){
    int status;
    int socket_fd;
    struct addrinfo host_addrinfo;
    struct addrinfo *host_addrinfo_list;
    memset(&host_addrinfo, 0, sizeof(host_addrinfo));
    host_addrinfo.ai_family   = AF_UNSPEC;
    host_addrinfo.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(host.c_str(), port.c_str(), &host_addrinfo, &host_addrinfo_list);
    if (status != 0) {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << host << "," << port << ")" << std::endl;
        return -1;
    } //if

    socket_fd = socket(host_addrinfo_list->ai_family, 
                host_addrinfo_list->ai_socktype, 
                host_addrinfo_list->ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << host << "," << port << ")" << std::endl;
        return -1;
    } //if

    std::cout << "Connecting to " << host << " on port " << port << "..." << std::endl;

    status = connect(socket_fd, host_addrinfo_list->ai_addr, host_addrinfo_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot connect to socket" << std::endl;
        std::cerr << "  (" << host << "," << port << ")" << std::endl;
        return -1;
    } //if
    freeaddrinfo(host_addrinfo_list);

    return socket_fd;
    }


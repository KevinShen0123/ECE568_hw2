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
    auto n = p.put(buf, ec);
    http::request<http::string_body> res = p.get();
    // *res = p.get();
    return res;
}

http::response<http::string_body> parseResponse(std::string message){
    beast::error_code ec;
    http::response_parser<http::string_body> p;

    auto buf = boost::asio::buffer(message.data(), message.size());
    auto n = p.put(buf, ec);
    http::response<http::string_body> res = p.get();
    // *res = p.get();
    return res;
}

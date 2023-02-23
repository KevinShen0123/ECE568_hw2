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

class Message{
private:
    bool isRequest;
    std::string message;
public:
    Message(bool r, std::string m): isRequest(r), message(m){}
    http::request<http::string_body> parseRequest(){
        beast::error_code ec;
        http::request_parser<http::string_body> p;

        auto buf = boost::asio::buffer(message.data(), message.size());
        auto n = p.put(buf, ec);
        http::request<http::string_body> res = p.get();
        // *res = p.get();
        return res;
    }

    http::response<http::string_body> parseResponse(){
        beast::error_code ec;
        http::response_parser<http::string_body> p;

        auto buf = boost::asio::buffer(message.data(), message.size());
        auto n = p.put(buf, ec);
        http::response<http::string_body> res = p.get();
        // *res = p.get();
        return res;
    }
};

// int main(){
//     std::string request = "GET /forms/post HTTP/1.1\r\n"
// "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
// "Accept-Encoding: gzip, deflate\r\n"
// "Accept-Language: zh-CN,zh;q=0.9,en-US;q=0.8,en;q=0.7\r\n"
// "Cache-Control: max-age=0\r\n"
// "Connection: keep-alive\r\n"
// "Host: httpbin.org\r\n"
// "Upgrade-Insecure-Requests: 1\r\n"
// "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36\r\n"
// "\r\n";
//     Message m(true,request);
//     http::request<http::string_body>  r = m.parseRequest();
//     std::cout << r.at("HOST") << std::endl;

// }
#pragma once
#include <boost/asio.hpp>
#include <memory>

using boost::asio::ip::tcp;

class Order;
class Book;

// this was created as shared ptr and we need later `this`
// therefore we need to inherit from enable_shared_from_this
class session : public std::enable_shared_from_this<session>
{
public:
    // our sesseion holds the socket
    session(tcp::socket socket, std::unordered_map<std::string, std::shared_ptr<Order>> orders, std::shared_ptr<Book> orderBook);

    // and run was already called in our server, where we just wait for requests
    void run();
private:
    void wait_for_request();
private:
    tcp::socket m_socket;
    boost::asio::streambuf m_buffer;
    std::unordered_map<std::string, std::shared_ptr<Order>> orders;
    std::shared_ptr<Book> orderBook;
};
class server
{
public:
    // we need an io_context and a port where we listen to 
    server(boost::asio::io_context& io_context, short port, std::unordered_map<std::string, std::shared_ptr<Order>> orders, std::shared_ptr<Book> orderBook);
private:
    void do_accept();
private:
    tcp::acceptor m_acceptor;
    std::unordered_map<std::string, std::shared_ptr<Order>> orders;
    std::shared_ptr<Book> orderBook;
};

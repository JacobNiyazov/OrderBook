#include <boost/asio.hpp>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    using boost::asio::ip::tcp;
    boost::asio::io_context io_context;

    // we need a socket and a resolver
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);

    // now we can use connect(..)
    boost::asio::connect(socket, resolver.resolve("127.0.0.1", "25000"));

    std::cout << "Enter your order command in the following format:\n";
    std::cout << "<command> <buy/sell> <number of shares> <limit price>\n";
    std::cout << "Where <command> can be 'add', 'execute', or 'cancel'.\n";
    std::cout << "Example: add buy 100 50.25\n";
    std::cout << "This will add an order to buy 100 shares at a limit price of $50.25.\n";

    // and use write(..) to send some data which is here just a string
    while (true)
    {
        std::string data;
        std::getline(std::cin, data);
        auto result = boost::asio::write(socket, boost::asio::buffer(data));

        // the result represents the size of the sent data
        std::cout << "data sent: " << result << '/' << data.length() << std::endl;

        if (data == "quit")
        {
            break;
        }
    }

    // and close the connection now
    boost::system::error_code ec;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket.close();

    return 0;
}

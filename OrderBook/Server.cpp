#include "Server.h"
#include "Order.h"
#include "Book.h"
#include "Print.h"
#include "Limit.h"

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <utility>

using boost::asio::ip::tcp;

void parseCommandInput(std::shared_ptr<Book> orderBook, std::string command, std::string& commandType, bool& isBuy, int& numShares, float& limit, std::string& id, std::string& errMsg)
{
    int pos = 0;
    std::string token;

    try
    {
        pos = command.find(" ");
        commandType = boost::algorithm::to_lower_copy(command.substr(0, pos));
        command.erase(0, pos + 1);

        if (commandType != "add" && commandType != "cancel" && commandType != "execute")
        {
            errMsg = "Invalid Command Given.";
            return;
        }
        
        if (commandType == "add")
        {
            pos = command.find(" ");
            std::string tempBuy = boost::algorithm::to_lower_copy(command.substr(0, pos));
            command.erase(0, pos + 1);

            if (tempBuy == "buy")
            {
                isBuy = true;
            }
            else if (tempBuy == "sell")
            {
                isBuy = false;
            }
            else
            {
                errMsg = "Invalid Buy/Sell Choice.";
                return;
            }

            pos = command.find(" ");
            try
            {
                numShares = stoi(command.substr(0, pos));
                if (numShares <= 0) throw 1;
            }
            catch (...)
            {
                errMsg = "Invalid number of shares.";
                return;
            }
            command.erase(0, pos + 1);

            try
            {
                limit = stof(command);
                if (limit <= 0.0) throw 1;
                if (isBuy && orderBook->getLowestSell() && limit > (orderBook->getLowestSell())->getLimitPrice()) throw 1;
                if (!isBuy && orderBook->getHighestBuy() && limit < (orderBook->getHighestBuy())->getLimitPrice()) throw 1;
            }
            catch (...)
            {
                errMsg = "Invalid limit price.";
                return;
            }
        }
        else
        {
            id = command;
        }

    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    
}



session::session(tcp::socket socket, std::unordered_map<std::string, std::shared_ptr<Order>> orders, std::shared_ptr<Book> orderBook)
    : m_socket(std::move(socket)), orders(orders), orderBook(orderBook) { }

void session::run() {
        wait_for_request();
}

void session::wait_for_request() {
    // since we capture `this` in the callback, we need to call shared_from_this()
    auto self(shared_from_this());
    // and now call the lambda once data arrives
    // we read a string until the null termination character
    boost::asio::async_read_until(m_socket, m_buffer, "\0",
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            // if there was no error, everything went well and for this demo
            // we print the data to stdout and wait for the next request
            if (!ec) {
                std::string command{
                    std::istreambuf_iterator<char>(&m_buffer),
                    std::istreambuf_iterator<char>()
                };
                // we just print the data, you can here call other api's 
                // or whatever the server needs to do with the received data
                std::cout << command << std::endl << std::endl;
                std::string commandType;
                bool isBuy;
                int  numShares;
                float limit;
                std::string id;
                std::string errMsg;
                parseCommandInput(orderBook, command, commandType, isBuy, numShares, limit, id, errMsg);

                if (!errMsg.empty())
                {
                    Print::println(errMsg);
                }
                else if (commandType == "cancel")
                {
                    if(orders.find(id) != orders.end())
                    {
                        bool success = orderBook->cancelOrder(orders[id]);
                        if (success)
                        {
                            orders.erase(id);
                        }
                        orderBook->displayBook();
                    }
                    else
                    {
                        Print::println("No order with this ID exists.");
                    }

                }
                else if (commandType == "execute")
                {
                    if (orders.find(id) != orders.end())
                    {
                        bool success = orderBook->executeOrder(orders[id]);
                        if (success && orders[id]->getShares() == 0)
                        {
                            orders.erase(id);
                        }
                        else
                        {
                            Print::println("Error executing entire order.");
                        }
                        orderBook->displayBook();
                    }
                    else
                    {
                        Print::println("No order with this ID exists.");
                    }
                }
                else
                {
                    std::shared_ptr<Order> temp;
                    temp = orderBook->addOrderToBook(isBuy, numShares, limit);
                    if (temp) 
                    { 
                        orders.insert({ temp->getId(), temp }); 
                        Print::println("Order ID: " + temp->getId());
                    }

                    orderBook->displayBook();
                }

                wait_for_request();
            }
            else {
                std::cout << "error: " << ec << std::endl;
                exit(1);
            }
        });
}


// we need an io_context and a port where we listen to 
server::server(boost::asio::io_context& io_context, short port, std::unordered_map<std::string, std::shared_ptr<Order>> orders, std::shared_ptr<Book> orderBook)
    : m_acceptor(io_context, tcp::endpoint(tcp::v4(), port)), orders(orders), orderBook(orderBook) {
    // now we call do_accept() where we wait for clients
    server::do_accept();
}
void server::do_accept() {
    // this is an async accept which means the lambda function is 
    // executed, when a client connects
    m_acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            // let's see where we created our session
            std::cout << "creating session on: "
                << socket.remote_endpoint().address().to_string()
                << ":" << socket.remote_endpoint().port() << '\n';
            // create a session where we immediately call the run function
            // note: the socket is passed to the lambda here
            std::make_shared<session>(std::move(socket), orders, orderBook)->run();
        }
        else {
            std::cout << "error: " << ec.message() << std::endl;
        }
        // since we want multiple clients to connnect, wait for the next one by calling do_accept()
        do_accept();
        });
}


#include "Book.h"
#include "Limit.h"
#include "Order.h"
#include "Print.h"
#include "Server.h"

#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>




int main(int argc, char* argv[])
{
    std::cout << std::fixed << std::setprecision(2);
    std::unordered_map<std::string, std::shared_ptr<Order>> orders;
    std::shared_ptr<Book> orderBook = std::make_shared<Book>();
    
    // here we create the io_context
    boost::asio::io_context io_context;
    // we'll just use an arbitrary port here 
    server s(io_context, 25000, orders, orderBook);
    // and we run until our server is alive
    io_context.run();

    /*std::shared_ptr<Order> temp;
    temp = orderBook->addOrderToBook(true, 100, 150);
    if (temp) { orders.insert({ temp->getId(), temp }); }
    temp = orderBook->addOrderToBook(true, 100, 150);
    if (temp) { orders.insert({ temp->getId(), temp }); }
    temp = orderBook->addOrderToBook(true, 10, 75);
    if (temp) { orders.insert({ temp->getId(), temp }); }
    temp = orderBook->addOrderToBook(true, 100, 200);
    if (temp) { orders.insert({ temp->getId(), temp }); }
    temp = orderBook->addOrderToBook(true, 100, 75);
    if (temp) { orders.insert({ temp->getId(), temp }); }
    temp = orderBook->addOrderToBook(false, 100, 250);
    if (temp) { orders.insert({ temp->getId(), temp }); }
    temp = orderBook->addOrderToBook(false, 25, 230);
    if (temp) { orders.insert({ temp->getId(), temp }); }
    temp = orderBook->addOrderToBook(false, 100, 200);
    if (temp) { orders.insert({ temp->getId(), temp }); }*/
    
    
    //for (auto i = orders.begin(); i != orders.end();) {
    //	if (i->second->getShares() <= 0) {
    //		i = orders.erase(i);  // Erase element and advance to the next
    //	}
    //	else {
    //		std::cout << i->first << ":" << i->second->getLimit() << " " << i->second->getShares() << "\n";
    //		++i;
    //	}
    //}
    
    //orderBook->displayBook();

    return 0;
}
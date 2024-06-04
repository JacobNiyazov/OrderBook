#pragma once

//#include "Limit.h"
//#include "Order.h"
#include <memory>

class Limit;
class Order;

class Book
{
private:
	std::shared_ptr<Limit> buyTree;
	std::shared_ptr<Limit> sellTree;
	std::shared_ptr<Limit> lowestSell;
	std::shared_ptr<Limit> highestBuy;

public:
    Book();

    std::shared_ptr<Order> addOrderToBook(bool isBuy, int shares, float limit);
    bool cancelOrder(std::shared_ptr<Order> order);
    bool executeOrder(std::shared_ptr<Order> order);
    static void displayBook(std::unique_ptr<Book>& book);
    void displayBook();

    std::shared_ptr<Limit> getBuyTree() const;
    std::shared_ptr<Limit> getSellTree() const;
    std::shared_ptr<Limit> getLowestSell() const;
    std::shared_ptr<Limit> getHighestBuy() const;

    void setBuyTree(std::shared_ptr<Limit> newBuyTree);
    void setSellTree(std::shared_ptr<Limit> newSellTree);
    void setLowestSell(std::shared_ptr<Limit> newLowestSell);
    void setHighestBuy(std::shared_ptr<Limit> newHighestBuy);
};
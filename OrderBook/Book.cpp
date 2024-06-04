#include "Book.h"
#include "Limit.h"
#include "Order.h"
#include "Print.h"

#include <memory>
#include <iostream>
#include <iomanip>
#include <vector>

//class Limit;
//class Order;


Book::Book()
    : buyTree(nullptr), sellTree(nullptr), lowestSell(nullptr), highestBuy(nullptr) {}

std::shared_ptr<Order> Book::addOrderToBook(bool isBuy, int shares, float limit)
{
    if (isBuy)
    {
        // Fullfill order
        if (lowestSell && limit == lowestSell->getLimitPrice())
        {
            lowestSell->deductFromLimit(shares);

            // if lowestSell Limit is empty, remove it from sellTree
            if (lowestSell->getSize() == 0 && lowestSell->getTotalVol() == 0)
            {
                sellTree = Limit::deleteLimit(sellTree, lowestSell);
                lowestSell = Limit::getMinLimit(sellTree);
            }
            // if all shares fulfilled, exit
            if (shares == 0)
            {
                return nullptr;
            }
        }
        // Check if limit is in buy tree
        std::shared_ptr<Limit> lim = Limit::limitExists(buyTree, limit);

        // Create Limit and add to buy tree
        if (lim == NULL)
        {
            buyTree = Limit::insertLimit(buyTree, limit);
            lim = Limit::limitExists(buyTree, limit);

            if (!highestBuy || limit > highestBuy->getLimitPrice())
            {
                highestBuy = lim;
            }
        }
        // Create new Order
        return lim->addOrderToLimit(isBuy, shares);
    }
    else
    {
        // Fullfill order
        if (highestBuy && limit == highestBuy->getLimitPrice())
        {
            highestBuy->deductFromLimit(shares);
            // if highestBuy Limit is empty, remove it from buyTree
            if (highestBuy->getSize() == 0 && highestBuy->getTotalVol() <= 0)
            {
                buyTree = Limit::deleteLimit(buyTree, highestBuy);

                highestBuy = Limit::getMaxLimit(buyTree);
            }
            // if all shares fulfilled, exit
            if (shares == 0)
            {
                return nullptr;
            }
        }
        // Check if limit is in sell tree
        std::shared_ptr<Limit> lim = Limit::limitExists(sellTree, limit);

        // Create Limit and add to sell tree
        if (lim == NULL)
        {
            sellTree = Limit::insertLimit(sellTree, limit);
            lim = Limit::limitExists(sellTree, limit);

            if (!lowestSell || limit < lowestSell->getLimitPrice())
            {
                lowestSell = lim;
            }
        }
        // Create new Order
        return lim->addOrderToLimit(isBuy, shares);
    }
}
bool Book::cancelOrder(std::shared_ptr<Order> order)
{
    std::shared_ptr<Limit> lim;
    bool success;
    if (order->getIsBuy())
    {
        lim = Limit::limitExists(buyTree, order->getLimit());
        success = lim->cancelOrderFromLimit(order->getId());
        if (lim->getSize() == 0)
        {
            Limit::deleteLimit(buyTree, lim);
        }
    }
    else
    {
        lim = Limit::limitExists(sellTree, order->getLimit());
        success = lim->cancelOrderFromLimit(order->getId());
        if (lim->getSize() == 0)
        {
            Limit::deleteLimit(sellTree, lim);
        }

    }    
    return success;
}
bool Book::executeOrder(std::shared_ptr<Order> order)
{
    int shares = order->getShares();
    while (shares > 0)
    {
        if (order->getIsBuy())
        {
            if (lowestSell == NULL) break;

            int vol = lowestSell->getTotalVol();
            if (shares < vol) vol = shares;
            addOrderToBook(true, vol, lowestSell->getLimitPrice());
            shares -= vol;
            order->setShares(shares);

        }
        else
        {
            if (highestBuy == NULL) break;

            int vol = highestBuy->getTotalVol();
            if (shares < vol) vol = shares;
            addOrderToBook(false, vol, highestBuy->getLimitPrice());
            shares -= vol;
            order->setShares(shares);

        }
    }
    if (shares == 0)
    {
        return cancelOrder(order);
    }
    return false;
}


void inOrderTraversal(std::shared_ptr<Limit> root) {
    if (root == nullptr) {
        return;
    }
    inOrderTraversal(root->getLeftChild()); // Traverse the left subtree
    std::cout << root->getLimitPrice() << " (Size:" << root->getSize() << ") "; // Visit the root
    inOrderTraversal(root->getRightChild()); // Traverse the right subtree
}
void Book::displayBook(std::unique_ptr<Book>& book)
{
    if(book->lowestSell)
    {
        std::cout << "\nLowest Sell: " << book->lowestSell->getLimitPrice() << " (Size:" << book->lowestSell->getSize() << ")" << std::endl;
    }

    if(book->sellTree)
    {
        std::cout << "Sell Tree: " << std::endl;
        inOrderTraversal(book->sellTree);
    }
    if(book->highestBuy)
    {
        std::cout << "\nHighest Buy: " << book->highestBuy->getLimitPrice() << " (Size:" << book->highestBuy->getSize() << ")" << std::endl;
    }
    if(book->buyTree)
    {
        std::cout << "Buy Tree: " << std::endl;
        inOrderTraversal(book->buyTree);
    }
}

void inOrder(std::shared_ptr<Limit> root, std::vector<std::shared_ptr<Limit>>& vec)
{
    if (root == nullptr) {
        return;
    }
    inOrder(root->getLeftChild(), vec); // Traverse the left subtree
    vec.emplace_back(root);
    inOrder(root->getRightChild(), vec); // Traverse the right subtree
}
void reverseInOrder(std::shared_ptr<Limit> root, std::vector<std::shared_ptr<Limit>>& vec)
{
    if (root == nullptr) {
        return;
    }
    reverseInOrder(root->getRightChild(), vec); // Traverse the right subtree
    vec.emplace_back(root);
    reverseInOrder(root->getLeftChild(), vec); // Traverse the left subtree
}
void Book::displayBook()
{
    std::vector<std::shared_ptr<Limit>> asks;
    std::vector<std::shared_ptr<Limit>> bids;
    inOrder(sellTree, asks);
    reverseInOrder(buyTree, bids);

    const int width = 15;

    // Print header
    Print::bookln("SIZE", "VOLUME", "BID PRICE", "OFFER PRICE", "VOLUME", "SIZE");

    int askIdx = 0, bidIdx = 0;

    while (askIdx < asks.size() || bidIdx < bids.size())
    {
        if (askIdx >= asks.size() && bidIdx < bids.size())
        {
            Print::bookln(bids[bidIdx]->getSize(), bids[bidIdx]->getTotalVol(), bids[bidIdx]->getLimitPrice(), "-", "-", "-");
            bidIdx++;
        }
        else if (askIdx < asks.size() && bidIdx >= bids.size())
        {
            Print::bookln("-", "-", "-", asks[askIdx]->getLimitPrice(), asks[askIdx]->getTotalVol(), asks[askIdx]->getSize());
            askIdx++;
        }
        else
        {
            Print::bookln(bids[bidIdx]->getSize(), bids[bidIdx]->getTotalVol(), bids[bidIdx]->getLimitPrice(), asks[askIdx]->getLimitPrice(), asks[askIdx]->getTotalVol(), asks[askIdx]->getSize());

            askIdx++;
            bidIdx++;
        }
    }
    std::string dashes(100, '-');
    std::cout << dashes << std::endl << std::endl;
}


std::shared_ptr<Limit> Book::getBuyTree() const { return buyTree; }
std::shared_ptr<Limit> Book::getSellTree() const { return sellTree; }
std::shared_ptr<Limit> Book::getLowestSell() const { return lowestSell; }
std::shared_ptr<Limit> Book::getHighestBuy() const { return highestBuy; }

void Book::setBuyTree(std::shared_ptr<Limit> newBuyTree) { buyTree = newBuyTree; }
void Book::setSellTree(std::shared_ptr<Limit> newSellTree) { sellTree = newSellTree; }
void Book::setLowestSell(std::shared_ptr<Limit> newLowestSell) { lowestSell = newLowestSell; }
void Book::setHighestBuy(std::shared_ptr<Limit> newHighestBuy) { highestBuy = newHighestBuy; }
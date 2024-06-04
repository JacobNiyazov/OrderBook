#include "Limit.h"
#include "Order.h"
#include "Print.h"

#include <memory>
#include <iostream>
//class Order;


Limit::Limit(float price)
    : limitPrice(price), size(0), totalVol(0), parent(nullptr), leftChild(nullptr), rightChild(nullptr) {}


std::shared_ptr<Limit> Limit::limitExists(std::shared_ptr<Limit> root, float price)
{
    if (root == NULL || root->limitPrice == price)
    {
        return root;
    }
    else if (root->limitPrice < price) {
        return limitExists(root->rightChild, price);
    }
    else
    {
        return limitExists(root->leftChild, price);
    }
}
std::shared_ptr<Limit> Limit::insertLimit(std::shared_ptr<Limit> root, float newLimit)
{
    if (root == NULL)
    {
        std::shared_ptr<Limit> createdLimit = std::make_shared<Limit>(newLimit);
        return createdLimit;
    }

    if (root->limitPrice > newLimit)
    {
        std::shared_ptr<Limit> lChild = insertLimit(root->leftChild, newLimit);
        root->leftChild = lChild;
        lChild->parent = root;
    }
    else if (root->limitPrice < newLimit)
    {
        std::shared_ptr<Limit> rChild = insertLimit(root->rightChild, newLimit);
        root->rightChild = rChild;
        rChild->parent = root;
    }

    return root;
}
std::shared_ptr<Limit> Limit::deleteLimit(std::shared_ptr<Limit> root, std::shared_ptr<Limit> limit)
{
    if (root == NULL)
    {
        return root;
    }

    if (root->limitPrice > limit->limitPrice)
    {
        root->leftChild = deleteLimit(root->leftChild, limit);
        return root;
    }
    else if (root->limitPrice < limit->limitPrice)
    {
        root->rightChild = deleteLimit(root->rightChild, limit);
        return root;
    }

    if (root->leftChild == NULL)
    {
        std::shared_ptr<Limit> temp = root->rightChild;
        if(temp)
        {
            temp->parent = nullptr;
        }
        return temp;
    }
    else if (root->rightChild == NULL)
    {
        std::shared_ptr<Limit> temp = root->leftChild;
        if (temp)
        {
            temp->parent = nullptr;
        }
        return temp;
    }
    else
    {
        std::shared_ptr<Limit> parent = root;
        std::shared_ptr<Limit> temp = root->rightChild;
        while (temp->leftChild != NULL)
        {
            parent = temp;
            temp = temp->leftChild;
        }

        if (parent == root)
        {
            parent->rightChild = temp->rightChild;
        }
        else
        {
            parent->leftChild = temp->rightChild;
        }

        temp->rightChild = root->rightChild;
        temp->leftChild = root->leftChild;
        temp->leftChild->parent = temp;
        temp->rightChild->parent = temp;
        return temp;

    }
}
std::shared_ptr<Limit> Limit::getMinLimit(std::shared_ptr<Limit> root)
{
    std::shared_ptr<Limit> minLim = root;
    while (minLim->leftChild)
    {
        minLim = minLim->leftChild;
    }
    return minLim;
}
std::shared_ptr<Limit> Limit::getMaxLimit(std::shared_ptr<Limit> root)
{
    std::shared_ptr<Limit> maxLim = root;
    while (maxLim && maxLim->rightChild)
    {
        maxLim = maxLim->rightChild;
    }
    return maxLim;
}
std::shared_ptr<Order> Limit::addOrderToLimit(bool isBuy, int shares)
{
    std::shared_ptr<Order> newOrder = std::make_shared<Order>(isBuy, shares, this->limitPrice, shared_from_this());
    // Orders for this Limit alread exist
    if (!headOrder.expired() && !tailOrder.expired())
    {
        std::shared_ptr<Order> tempTailOrder = tailOrder.lock();
        newOrder->setPrevOrder(tempTailOrder);
        tempTailOrder->setNextOrder(newOrder);
        tailOrder = newOrder;
    }
    else
    {
        headOrder = newOrder;
        tailOrder = newOrder;
    }
    size += 1;
    totalVol += shares;
    return newOrder;
}
bool Limit::cancelOrderFromLimit(std::string id)
{
    std::shared_ptr<Order> curr = headOrder.lock();
    while (curr && curr->getId() != id)
    {
        curr = curr->getNextOrder();
    }
    if (curr == headOrder.lock())
    {
        headOrder = curr->getNextOrder();
        totalVol -= curr->getShares();
        size -= 1;
        return true;
    }
    if (curr == tailOrder.lock())
    {
        tailOrder = curr->getPrevOrder();
        totalVol -= curr->getShares();
        size -= 1;
        return true;
    }
    try
    {
        std::shared_ptr<Order> prev = curr->getPrevOrder();
        prev->setNextOrder(curr->getNextOrder());
        (curr->getNextOrder())->setPrevOrder(prev);
        totalVol -= curr->getShares();
        size -= 1;
        return true;
    }
    catch (...)
    {
        Print::println("Error canceling order.");
        return false;
    }
}
void Limit::deductFromLimit(int& shares)
{
    while (shares > 0 && !headOrder.expired())
    {
        std::shared_ptr<Order> tempHeadOrder = headOrder.lock();
        int unfulfilledShares = tempHeadOrder->fulfillOrder(shares);
        totalVol -= (shares - unfulfilledShares);
        shares = unfulfilledShares;
        // iterate headOrder to next Order
        if (tempHeadOrder->getShares() <= 0)
        {
            headOrder = tempHeadOrder->getNextOrder();
            size -= 1;
            if (headOrder.expired())
            {
                tailOrder.reset();
                break;
            }
            tempHeadOrder->setPrevOrder(nullptr);
        }

    }
}

float Limit::getLimitPrice() const { return limitPrice; }
int Limit::getSize() const { return size; }
int Limit::getTotalVol() const { return totalVol; }
std::shared_ptr<Limit> Limit::getParent() const { return parent; }
std::shared_ptr<Limit> Limit::getLeftChild() const { return leftChild; }
std::shared_ptr<Limit> Limit::getRightChild() const { return rightChild; }
std::weak_ptr<Order> Limit::getHeadOrder() const { return headOrder; }
std::weak_ptr<Order> Limit::getTailOrder() const { return tailOrder; }

void Limit::setLimitPrice(float newLimitPrice) { limitPrice = newLimitPrice; }
void Limit::setSize(int newSize) { size = newSize; }
void Limit::setTotalVol(int newTotalVol) { totalVol = newTotalVol; }
void Limit::setParent(std::shared_ptr<Limit> newParent) { parent = newParent; }
void Limit::setLeftChild(std::shared_ptr<Limit> newLeftChild) { leftChild = newLeftChild; }
void Limit::setRightChild(std::shared_ptr<Limit> newRightChild) { rightChild = newRightChild; }
void Limit::setHeadOrder(std::weak_ptr<Order> newHeadOrder) { headOrder = newHeadOrder; }
void Limit::setTailOrder(std::weak_ptr<Order> newTailOrder) { tailOrder = newTailOrder; }
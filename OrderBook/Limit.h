#pragma once

#include <memory>
#include <string>
//#include "Order.h"
class Order;

class Limit : public std::enable_shared_from_this<Limit>
{
private:
	float limitPrice;
	int size;
	int totalVol;
	std::shared_ptr<Limit> parent;
	std::shared_ptr<Limit> leftChild;
	std::shared_ptr<Limit> rightChild;
	std::weak_ptr<Order> headOrder;
	std::weak_ptr<Order> tailOrder;
public:
    Limit(float price);

    static std::shared_ptr<Limit> limitExists(std::shared_ptr<Limit> root, float price);
    static std::shared_ptr<Limit> insertLimit(std::shared_ptr<Limit> root, float newLimit);
    static std::shared_ptr<Limit> deleteLimit(std::shared_ptr<Limit> root, std::shared_ptr<Limit> limit);
    static std::shared_ptr<Limit> getMinLimit(std::shared_ptr<Limit> root);
    static std::shared_ptr<Limit> getMaxLimit(std::shared_ptr<Limit> root);
    std::shared_ptr<Order> addOrderToLimit(bool isBuy, int shares);
    bool cancelOrderFromLimit(std::string id);
    void deductFromLimit(int& shares);

    float getLimitPrice() const;
    int getSize() const;
    int getTotalVol() const;
    std::shared_ptr<Limit> getParent() const;
    std::shared_ptr<Limit> getLeftChild() const;
    std::shared_ptr<Limit> getRightChild() const;
    std::weak_ptr<Order> getHeadOrder() const;
    std::weak_ptr<Order> getTailOrder() const;

    void setLimitPrice(float newLimitPrice);
    void setSize(int newSize);
    void setTotalVol(int newTotalVol);
    void setParent(std::shared_ptr<Limit> newParent);
    void setLeftChild(std::shared_ptr<Limit> newLeftChild);
    void setRightChild(std::shared_ptr<Limit> newRightChild);
    void setHeadOrder(std::weak_ptr<Order> newHeadOrder);
    void setTailOrder(std::weak_ptr<Order> newTailOrder);
};
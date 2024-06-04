#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/chrono.hpp>
#include <memory>

//#include "Limit.h"
class Limit;

class Order
{
private:
	boost::uuids::uuid id;
	bool isBuy;
	int shares;
	float limit;
    boost::chrono::high_resolution_clock::time_point entryTime;
    boost::chrono::high_resolution_clock::time_point eventTime;
	std::shared_ptr<Order> nextOrder;
	std::shared_ptr<Order> prevOrder;
	std::weak_ptr<Limit> parentLimit;
public:
    Order(bool _isBuy, int _shares, float _limit, std::weak_ptr<Limit> _parentLimit);

    int fulfillOrder(int numShares);

    std::string getId() const;
    bool getIsBuy() const;
    int getShares() const;
    float getLimit() const;
    boost::chrono::high_resolution_clock::time_point getEntryTime() const;
    boost::chrono::high_resolution_clock::time_point getEventTime() const;
    std::shared_ptr<Order> getNextOrder() const;
    std::shared_ptr<Order> getPrevOrder() const;
    std::weak_ptr<Limit> getParentLimit() const;

    //void setId(int newId);
    void setIsBuy(bool newIsBuy);
    void setShares(int newShares);
    void setLimit(float newLimit);
    //void setEntryTime(int newEntryTime);
    void setEventTime(boost::chrono::high_resolution_clock::time_point newEventTime);
    void setNextOrder(std::shared_ptr<Order> newNextOrder);
    void setPrevOrder(std::shared_ptr<Order> newPrevOrder);
    void setParentLimit(std::weak_ptr<Limit> newParentLimit);

};
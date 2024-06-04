#include "Order.h"
#include "Limit.h"
//#include "Print.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/chrono.hpp>
#include <string>
#include <sstream>

//class Limit;


Order::Order(bool _isBuy, int _shares, float _limit, std::weak_ptr<Limit> _parentLimit)
    : isBuy(_isBuy), shares(_shares), limit(_limit), nextOrder(nullptr), prevOrder(nullptr), parentLimit(_parentLimit)
{
    static boost::uuids::random_generator generator;
    id = generator();
    entryTime = boost::chrono::high_resolution_clock::now();
}
int Order::fulfillOrder(int numShares)
{
    eventTime = boost::chrono::high_resolution_clock::now();

    int res = numShares - shares > 0 ? numShares - shares : 0;
    shares -= numShares;
    return res;

}

std::string Order::getId() const
{
    std::stringstream ss;
    ss << id;
    return ss.str();
}

bool Order::getIsBuy() const { return isBuy; }
int Order::getShares() const { return shares; }
float Order::getLimit() const { return limit; }
boost::chrono::high_resolution_clock::time_point Order::getEntryTime() const { return entryTime; }
boost::chrono::high_resolution_clock::time_point Order::getEventTime() const { return eventTime; }
std::shared_ptr<Order> Order::getNextOrder() const { return nextOrder; }
std::shared_ptr<Order> Order::getPrevOrder() const { return prevOrder; }
std::weak_ptr<Limit> Order::getParentLimit() const { return parentLimit; }

//void Order::setId(int newId) { id = newId; }
void Order::setIsBuy(bool newIsBuy) { isBuy = newIsBuy; }
void Order::setShares(int newShares) { shares = newShares; }
void Order::setLimit(float newLimit) { limit = newLimit; }
//void Order::setEntryTime(int newEntryTime) { entryTime = newEntryTime; }
void Order::setEventTime(boost::chrono::high_resolution_clock::time_point newEventTime) { eventTime = newEventTime; }
void Order::setNextOrder(std::shared_ptr<Order> newNextOrder) { nextOrder = newNextOrder; }
void Order::setPrevOrder(std::shared_ptr<Order> newPrevOrder) { prevOrder = newPrevOrder; }
void Order::setParentLimit(std::weak_ptr<Limit> newParentLimit) { parentLimit = newParentLimit; }

#include <iostream>
#include <string>
#include "stl_custom/memory/object_pool.hpp"

struct SensorReading {
    double value_;
    std::string name_;
    SensorReading(double value, const std::string& name) : value_(value), name_(name) {}
};

int main(){
    stl_custom::ObjectPool<SensorReading, 2> pool;
    auto h1 = pool.acquire(1, "Temperature");
    auto h2 = pool.acquire(2, "Gaz");
    if (h1) std::cout << "sensor name :" << (*h1)->name_ << '\n';
    if (h2) std::cout << "sensor name : " << (*h2)->name_ << '\n';
    return 0;
}
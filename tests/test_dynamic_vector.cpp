#include <iostream>
#include "stl_custom/containers/dynamic_vector.hpp"


int main() {
stl_custom::dynamic_vector<int> v;

int a = 9;

v.push_back(1);
v.push_back(2);
v.push_back(3);

v.reserve(6);
v.pop_back();
v.emplace_back(7);
v.push_back(a);
v.push_back(6); v.push_back(6); v.push_back(8);
v.push_back(10);

stl_custom::dynamic_vector<int> b = v;

stl_custom::random_access_iterator<int> d = v.begin();
stl_custom::random_access_iterator<const int> c = d;

return 0;
}

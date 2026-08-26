#include <iostream>
#include <string>
#include "stl_custom/containers/static_vector.hpp"

int main(int argc, char* argv[]){

stl_custom::static_vector<std::string ,12> arr;
arr.push_back(std::string("test1"));
arr.push_back("test2");
arr.emplace_back("test3");

for(auto it=arr.begin(); it!= arr.end(); ++it){
	std::cout << *it << " ";
}
std::cout << '\n';

std::cout << "static_array size : "<< arr.size() <<", static_array capacity : " << arr.capacity() <<'\n';

return 0;
}

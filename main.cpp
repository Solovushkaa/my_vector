#include "my_vector.h"
#include <iostream>
#include <vector>

int main(){
    std::vector<int> v;
    my_vector<int> mv(12,4);
    auto it = mv.begin();
    //++it;
    *it = 12;
    it = it + 4;
    *it = 8;
    std::cout << *(mv.begin()) << "\n";
    for (auto x : mv)
    {
        std::cout << x << ' ';
    }
    
}
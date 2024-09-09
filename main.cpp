#include "my_vector.h"
#include <iostream>

int main(){
    my_vector<int> mv(12ull,4);
    std::cout << *(mv.begin());
}
#include <initializer_list>
#include <type_traits>
#include <stdexcept>
#include <iterator>
#include <iostream>
#include <cstddef>
#include <utility>
#include <memory>

template<typename T,
         typename Allocator = std::allocator<T>
>class my_vector{
private:
    T* data = nullptr;
    Allocator allocator;
    size_t size = 0;
    size_t capacity = 0;
public:
    //constructors
    explicit my_vector() noexcept(noexcept(Allocator()));
    explicit my_vector(const Allocator& alloc) noexcept;
    my_vector(  size_t count, 
                const T& value, 
                const Allocator& alloc = Allocator() );
    my_vector(  size_t count,
                const Allocator& alloc = Allocator() );
    my_vector(  const my_vector& other);
    my_vector(  const my_vector& other,
                const Allocator& alloc);
    my_vector(  my_vector&& move_other);
    my_vector(  my_vector&& move_other,
                const Allocator& alloc);
    my_vector(  std::initializer_list<T> init,
                const Allocator& alloc = Allocator());

    /*constructors from iterators
    *
    */

    //functions
    


    //operators
    bool operator<(const my_vector& b) = delete;
    bool operator>(const my_vector& b) = delete;
    bool operator<=(const my_vector& b) = delete;
    bool operator>=(const my_vector& b) = delete;

    bool operator==(const my_vector& b);
    bool operator!=(const my_vector& b) { return !(*this != b); }

    my_vector& operator[](size_t pos) { return data[pos]; }
    my_vector& operator[](size_t pos) const { return data[pos]; }

    //destructor
    ~my_vector();
};
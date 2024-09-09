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
    T* arr_ = nullptr;
    Allocator alloc_;
    size_t sz_ = 0;
    size_t cap_ = 0;
private:
    template<bool IsConst>
    class base_iterator{
    public:
        using iterator_category = std::random_access_iterator_tag;
        using pointer_type = std::conditional<IsConst, const T*, T*>;
        using reference_type = std::conditional<IsConst, const T&, T&>;
        using value_type = T;

    private:
        pointer_type ptr;

    public:
        base_iterator(pointer_type ptr): ptr(ptr) {}
        base_iterator(const base_iterator&) = default;
        base_iterator& operator=(const base_iterator&) = default;

        reference_type operator*() { return *ptr; }
        pointer_type operator->() { return ptr; }

        reference_type operator+(int val) {
            ptr += val;
            return *this; 
        }
        reference_type operator-(int val) { 
            ptr -= val;
            return *this; 
        }

        reference_type operator++() { 
            ++ptr;
            return ptr; 
        }
        reference_type operator++(int) {
            base_iterator copy = *this;
            ++ptr;
            return copy;
        }
        reference_type operator--() { 
            --ptr;
            return ptr; 
        }
        reference_type operator--(int) { 
            base_iterator copy = *this;
            --ptr;
            return copy; 
        }

        reference_type operator+=(int val) { 
            ptr += val;
            return *this; 
        }
        reference_type operator-=(int val) { 
            ptr -= val;
            return *this; 
        }
    };

public:
    //iterators
    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;

    iterator begin() { return iterator(arr_); }
    iterator end() { return iterator(arr_+sz_); }

    const_iterator begin() const { return const_iterator(arr_); }
    const_iterator end() const { return const_iterator(arr_+sz_); }

    const_iterator cbegin() const { return const_iterator(arr_); }
    const_iterator cend() const { return const_iterator(arr_+sz_); }

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
                const Allocator& alloc = Allocator() );
    template<typename InputIt>
    my_vector(  InputIt first, InputIt last,
                const Allocator& alloc = Allocator() );

    /*constructors from base_iterators
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

    my_vector& operator[](size_t pos) { return arr_[pos]; }
    my_vector& operator[](size_t pos) const { return arr_[pos]; }

    //destructor
    ~my_vector();
};
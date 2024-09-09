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
    T* arr_;
    Allocator alloc_;
    size_t sz_;
    size_t cap_;
private:
    template<bool IsConst>
    class base_iterator{
    public:
        using iterator_category = std::random_access_iterator_tag;
        using pointer_type = typename std::conditional<IsConst, const T*, T*>::type;
        // using pointer_type = T*;
        using reference_type = typename std::conditional<IsConst, const T&, T&>::type;
        // using reference_type = T&;
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
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() noexcept { return iterator(arr_); }
    iterator end() noexcept { return iterator(arr_+sz_); }

    const_iterator begin() const noexcept { return const_iterator(arr_); }
    const_iterator end() const noexcept { return const_iterator(arr_+sz_); }

    const_iterator cbegin() const noexcept { return const_iterator(arr_); }
    const_iterator cend() const noexcept { return const_iterator(arr_+sz_); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(arr_+sz_-1); }
    reverse_iterator rend() noexcept { return reverse_iterator(arr_-1); }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(arr_+sz_-1); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(arr_-1); }

    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(arr_+sz_-1); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(arr_-1); }

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

    //functions
    //void push

    //operators

    bool operator==(const my_vector& b);
    bool operator!=(const my_vector& b) { return !(*this != b); }

    my_vector& operator[](size_t pos) { return arr_[pos]; }
    my_vector& operator[](size_t pos) const { return arr_[pos]; }

    //destructor
    ~my_vector(){
        for (size_t i = 0; i < sz_; ++i)
        {
            alloc_.destroy(arr_+i);
            (arr_+i)->~T();
        }
        alloc_.deallocate(arr_, cap_);
    }
};

template <typename T, typename Allocator>
my_vector<T, Allocator>::my_vector() noexcept(noexcept(Allocator())) :  arr_(nullptr)
                                                                        ,sz_(0)
                                                                        ,cap_(0){}

template <typename T, typename Allocator>
my_vector<T, Allocator>::my_vector(const Allocator &alloc) noexcept :   arr_(nullptr)
                                                                        ,sz_(0)
                                                                        ,cap_(0)
                                                                        ,alloc_(alloc){}

template <typename T, typename Allocator>
my_vector<T, Allocator>::my_vector(size_t count, const T &value, const Allocator &alloc) :  arr_(nullptr)
                                                                                            ,sz_(count)
                                                                                            ,cap_(count)
                                                                                            ,alloc_(alloc)
{
    if(!count) { return; }
    arr_ = alloc_.allocate(count);
    size_t index = 0;
    try
    {
        for (size_t index = 0; index < count; ++index)
        {
            alloc_.construct(arr_+index, value);
        }
    }
    catch(...)
    {
        for (size_t oldindex = 0; oldindex < index; ++oldindex)
        {
            alloc_.destroy(arr_+oldindex);
            (arr_+oldindex)->~T();
        }
        alloc_.deallocate(arr_, count);
        throw std::bad_alloc();
    }
}

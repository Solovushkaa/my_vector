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
        using pointer = typename std::conditional<IsConst, const T*, T*>::type;
        using reference = typename std::conditional<IsConst, const T&, T&>::type;
        using value_type = T;
        using difference_type = std::ptrdiff_t;

    private:
        pointer ptr;

    public:
        base_iterator(pointer ptr): ptr(ptr) {}
        base_iterator(const base_iterator&) = default;
        base_iterator& operator=(const base_iterator&) = default;

        reference operator*() noexcept { return *ptr; }
        pointer operator->() noexcept { return ptr; }

        base_iterator operator+(size_t val) noexcept {
            ptr += val;
            return *this; 
        }
        base_iterator operator-(size_t val) noexcept { 
            ptr -= val;
            return *this; 
        }

        base_iterator operator++() noexcept { 
            ++ptr;
            return *this; 
        }
        base_iterator operator++(int) noexcept {
            base_iterator copy = *this;
            ++ptr;
            return copy;
        }
        base_iterator operator--() noexcept { 
            --ptr;
            return *this; 
        }
        base_iterator operator--(int) noexcept { 
            base_iterator copy = *this;
            --ptr;
            return copy; 
        }

        base_iterator operator+=(int val) noexcept { 
            ptr += val;
            return *this; 
        }
        base_iterator operator-=(int val) noexcept { 
            ptr -= val;
            return *this; 
        }

        bool operator==(const base_iterator& it) const noexcept { 
            return ptr == it.ptr; 
        }
        bool operator!=(const base_iterator& it) const noexcept {
            return !(*this == it.ptr);  
        }
    };

public:
    //iterators
    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    [[nodiscard]] iterator begin() noexcept { return iterator(arr_); }
    [[nodiscard]] iterator end() noexcept { return iterator(arr_+sz_); }

    [[nodiscard]] const_iterator begin() const noexcept { return const_iterator(arr_); }
    [[nodiscard]] const_iterator end() const noexcept { return const_iterator(arr_+sz_); }

    [[nodiscard]] const_iterator cbegin() const noexcept { return const_iterator(arr_); }
    [[nodiscard]] const_iterator cend() const noexcept { return const_iterator(arr_+sz_); }

    [[nodiscard]] reverse_iterator rbegin() noexcept { return reverse_iterator(arr_+sz_-1); }
    [[nodiscard]] reverse_iterator rend() noexcept { return reverse_iterator(arr_-1); }

    [[nodiscard]] const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(arr_+sz_-1); }
    [[nodiscard]] const_reverse_iterator rend() const noexcept { return const_reverse_iterator(arr_-1); }

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(arr_+sz_-1); }
    [[nodiscard]] const_reverse_iterator crend() const noexcept { return const_reverse_iterator(arr_-1); }

    //constructors
    explicit my_vector() noexcept(noexcept(Allocator()));
    explicit my_vector(const Allocator& alloc) noexcept;
    my_vector(  int count, 
                const T& value, 
                const Allocator& alloc = Allocator() );
    my_vector(  int count,
                const Allocator& alloc = Allocator() );
    my_vector(  const my_vector& other);
    my_vector(  const my_vector& other,
                const Allocator& alloc);
    my_vector(  my_vector&& move_other);
    my_vector(  my_vector&& move_other,
                const Allocator& alloc);
    // my_vector(  std::initializer_list<T> init,
    //             const Allocator& alloc = Allocator() );
    template<typename It>
    my_vector(  It first, It last,
                const Allocator& alloc = Allocator() );

    //functions
    bool empty() const noexcept { return sz_; }
    size_t size() const noexcept { return sz_; }
    size_t capacity() const noexcept { return cap_; }
    void push_back(T&& obj);

    //operators

    bool operator==(const my_vector& b) { return (arr_ == b.arr_); }
    bool operator!=(const my_vector& b) { return !(*this == b); }



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
my_vector<T, Allocator>::my_vector(int count, const T &value, const Allocator &alloc) : arr_(nullptr)
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

template <typename T, typename Allocator>
my_vector<T, Allocator>::my_vector(int count, const Allocator &alloc) : arr_(nullptr)
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
            alloc_.construct(arr_+index, 0);
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

template <typename T, typename Allocator>
my_vector<T, Allocator>::my_vector(const my_vector &other) :    sz_(other.sz_)
                                                                ,cap_(other.cap_)
{
    if constexpr(std::is_fundamental_v<T> && !std::is_reference_v<T>){
        if(other.sz_ != 0){
            arr_ = alloc_.allocate(cap_);
            memcpy(arr_, other.arr_, sz_*sizeof(T));
        }
    }
    else{
        arr_ = alloc_.allocate(cap_);
        size_t index = 0;
        try
        {
            for (size_t index = 0; index < sz_; ++index)
            {
                alloc_.construct(arr_+index, 0);
            }
        }
        catch(...)
        {
            for (size_t oldindex = 0; oldindex < index; ++oldindex)
            {
                alloc_.destroy(arr_+oldindex);
                (arr_+oldindex)->~T();
            }
            alloc_.deallocate(arr_, cap_);
            throw std::bad_alloc();
        }
    }
}

template <typename T, typename Allocator>
my_vector<T, Allocator>::my_vector(const my_vector &other, const Allocator &alloc) :    sz_(other.sz_)
                                                                                        ,cap_(other.cap_)
                                                                                        ,alloc_(alloc)
{
    if constexpr(std::is_fundamental_v<T> && !std::is_reference_v<T>){
        if(other.sz_ != 0){
            arr_ = alloc_.allocate(cap_);
            memcpy(arr_, other.arr_, sz_*sizeof(T));
        }
    }
    else{
        arr_ = alloc_.allocate(cap_);
        size_t index = 0;
        try
        {
            for (size_t index = 0; index < sz_; ++index)
            {
                alloc_.construct(arr_+index, other[index]);
            }
        }
        catch(...)
        {
            for (size_t oldindex = 0; oldindex < index; ++oldindex)
            {
                alloc_.destroy(arr_+oldindex);
                (arr_+oldindex)->~T();
            }
            alloc_.deallocate(arr_, cap_);
            throw std::bad_alloc();
        }
    }
}

template <typename T, typename Allocator>
my_vector<T, Allocator>::my_vector(my_vector &&move_other) :    arr_(move_other.arr_)
                                                                ,sz_(move_other.sz_)
                                                                ,cap_(move_other.cap_)
{
    move_other.arr_ = nullptr;
    move_other.sz_ = 0;
    move_other.cap_ = 0;
}

template <typename T, typename Allocator>
my_vector<T, Allocator>::my_vector(my_vector &&move_other, const Allocator &alloc) :    arr_(move_other.arr_)
                                                                                        ,sz_(move_other.sz_)
                                                                                        ,cap_(move_other.cap_)
                                                                                        ,alloc_(alloc)
{
    move_other.arr_ = nullptr;
    move_other.sz_ = 0;
    move_other.cap_ = 0;
}


// template <typename T, typename Allocator>
// my_vector<T, Allocator>::my_vector(std::initializer_list<T> init, const Allocator &alloc) : sz_(init.size())
//                                                                                             ,cap_(init.size())
//                                                                                             ,alloc_(alloc)
// {
//     if constexpr(std::is_fundamental_v<T> && !std::is_reference_v<T>){
//         if(init.sz_ != 0){
//             arr_ = alloc_.allocate(cap_);
//             memcpy(arr_, init && ..., sz_*sizeof(T));
//         }
//     }
//     else{
//         arr_ = alloc_.allocate(cap_);
//         size_t index = 0;
//         try
//         {
//             for (size_t index = 0; index < init.size(); ++index)
//             {
//                 alloc_.construct(arr_+index, *(init.begin()+index));
//             }
//         }
//         catch(...)
//         {
//             for (size_t oldindex = 0; oldindex < index; ++oldindex)
//             {
//                 alloc_.destroy(arr_+oldindex);
//                 (arr_+oldindex)->~T();
//             }
//             alloc_.deallocate(arr_, cap_);
//             throw std::bad_alloc();
//         }
//     }
// }

template <typename T, typename Allocator>
template <typename It>
my_vector<T, Allocator>::my_vector(It first, It last, const Allocator &alloc) : sz_(std::distance(first, last))
                                                                                ,cap_(sz_)
                                                                                ,alloc_(alloc)
{
    arr_ = alloc_.allocate(cap_);
    size_t index = 0;
    try
    {
        for (auto it = first; it != last; ++it, ++index)
        {
            alloc_.construct(arr_+index, *(it+index));
        }
    }
    catch(...)
    {
        for (size_t oldindex = 0; oldindex < index; ++oldindex)
        {
            alloc_.destroy(arr_+oldindex);
            (arr_+oldindex)->~T();
        }
        alloc_.deallocate(arr_, cap_);
        throw std::bad_alloc();
    }
}

template <typename T, typename Allocator>
void my_vector<T, Allocator>::push_back(T &&obj)
{
    if(cap_ > sz_){
        alloc_.construct(arr_+sz_, obj)
    }
    else{
        size_t new_cap = cap_*2;
        T* new_arr = reserve(new_cap);
        for(size_t i = 0; i < sz_; ++i){
            new_arr[i] = std::move(arr_);
        }
        new_arr[sz_] = obj;
        delete[] arr_;
        arr_ = new_arr;
    }
}
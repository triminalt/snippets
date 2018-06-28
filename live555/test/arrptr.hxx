//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2017-09-02
//


#ifndef ARRPTR_HXX
#define ARRPTR_HXX


#include <cstddef>
#include <type_traits>
#include <memory>


template<typename T>
class arrptr {
public:
    using self_type = arrptr<T>;
    using element_type = T;
    using size_type = std::size_t;
public:
    template<typename TT>
    struct default_deleter {
        using self_type = default_deleter<TT>;

        default_deleter() = default;
        template< typename X
                , typename
                = typename std::enable_if_t< std::is_convertible<X*, TT*>::value
                                           , void>>
        default_deleter(default_deleter<X>const&) throw() = default;

        void operator() (TT* ptr) const throw() {
            static_assert(0 < sizeof(TT), "can't delete an incomplete type");
            delete [] ptr;
        }
    };
    template<typename TT>
    struct trivial_deleter {
        using self_type = trivial_deleter<TT>;

        trivial_deleter() = default;
        template< typename X
                , typename
                = typename std::enable_if_t< std::is_convertible<X*, TT*>::value
                                           , void>>
        trivial_deleter(trivial_deleter<X>const&) throw() = default;

        void operator() (TT* ptr) const throw() {
            static_assert(0 < sizeof(TT), "can't delete an incomplete type");
        }
    };
public:
    static arrptr nil() {
        return arrptr();
    }
    static arrptr make(std::size_t size, bool zero = true) {
        return arrptr(size, zero);
    }
    static arrptr own(element_type* ptr, std::size_t size) {
        return arrptr(ptr, size, default_deleter<element_type>());
    }
    static arrptr wrap(element_type* ptr, std::size_t size) {
        return arrptr(ptr, size, trivial_deleter<element_type>());
    }

public:
    arrptr() = default;
    ~arrptr() = default;
private:
    explicit arrptr(size_type size, bool zero = true)
        : ptr_(ptr(size, zero), default_deleter<element_type>())
        , size_(size) {
        // Empty.
    }
    template<typename D = trivial_deleter<element_type>>
    arrptr(element_type* ptr, size_type size, D deleter = D{})
        : ptr_(fix_ptr(ptr, size), deleter)
        , size_(fix_size(ptr, size)) {
        // Empty.
    }
public:
    element_type* ptr() const {
        return ptr_.get();
    }
    size_type size() const {
        return size_;
    }
    self_type& reset() {
        ptr_.reset();
        size_ = 0;
        return *this;
    }
    self_type& reset(size_type size, bool zero = true) {
        auto const ptr = ptr(size, zero);
        ptr_.reset(fix_ptr(ptr, size));
        size_ = fix_size(ptr, size);
        return *this;
    }
    self_type& reset(element_type* ptr, std::size_t size, bool owned = false) {
        if (owned) {
            ptr_.reset(fix_ptr(ptr, size), default_deleter<element_type>());
        } else {
            ptr_.reset(fix_ptr(ptr, size), trivial_deleter<element_type>());
        }
        size_ = fix_size(ptr, size);
        return *this;
    }
    template<typename D>
    self_type& reset(element_type* ptr, std::size_t size, D deleter) {
        ptr_.reset(fix_ptr(ptr, size), deleter);
        size_ = fix_size(ptr, size);
        return *this;
    }
    explicit operator bool () const throw() {
        return ptr_ && check(ptr_.get(), size_);
    }
private:
    inline static element_type* fix_ptr(element_type* ptr, size_type size) {
        return check(ptr, size) ? ptr : nullptr;
    }
    inline static size_t fix_size(element_type* ptr, size_type size) {
        return check(ptr, size) ? size : 0;
    }
    inline static bool check(element_type* ptr, size_type size) {
        return nullptr != ptr && 0 != size;
    }
    inline static element_type* ptr(size_type size, bool zero = true) {
        return 0 == size ? nullptr
                         : (zero ? new element_type[size]()
                                 : new element_type[size]);
    }
private:
    std::shared_ptr<element_type> ptr_;
    size_type size_ = 0;
};


#endif // ARRPTR_HXX

//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-05-21
//


#ifndef ZBB_CONFIG_PROPERTIES_HH
#define ZBB_CONFIG_PROPERTIES_HH


#include <iostream>
#include <tuple>
#include <json.hpp>
#include "../util/static_iterate.hh"
#include "./ns.hh"

#define ZBB_CONFIG_PROPERTIES_FORMAT_PRETTY 1


ZBB_CONFIG_BEGIN

template<typename K, typename ... Ts>
struct properties_codec {
    properties_codec() = delete;
    ~properties_codec() = delete;

    using key_type = K;
    using value_type = std::tuple<Ts ...>;
    using self_type = properties_codec<K, Ts ...>;

    struct to_json_predicate {
        explicit to_json_predicate(nlohmann::json& j)
            : j_(j) {
            // EMPTY
        }
        template<std::size_t I, typename E>
        bool apply(E const& element) {
            try {
                auto const key = key_type:: template get<I>();
                j_[key] = element;
                return true;
            } catch (std::exception const& e) {
                std::cerr << "to_json_predicate error: "
                          << e.what()
                          << std::endl;
                return false;
            }
        }
    private:
        ::nlohmann::json& j_;
    };

    struct from_json_predicate {
        explicit from_json_predicate(nlohmann::json const& j)
            : j_(j) {
            // EMPTY
        }
        template<std::size_t I, typename E>
        bool apply(E& element) {
            try {
                auto const key = key_type:: template get<I>();
                element = j_.at(key). template get<E>();
                return true;
            } catch (std::exception const& e) {
                std::cerr << "from_json_predicate error: "
                          << e.what()
                          << std::endl;
                return false;
            }
            return false;
        }
    private:
        nlohmann::json const& j_;
    };

    static value_type from_json(::nlohmann::json const& j) {
        if (j.is_null()) {
            return value_type{};
        }
        value_type v;
        if (!::zbb::util::static_iterate(v, from_json_predicate{j})) {
            return value_type{};
        } else {
            return v;
        }
    }

    static ::nlohmann::json to_json(value_type const& v) {
        nlohmann::json j;
        if (!::zbb::util::static_iterate(v, to_json_predicate{j})) {
            return ::nlohmann::json{};
        }
        return j;
    }

    static std::string encode(value_type const& v) {
        auto const j = to_json(v);
        if (j.is_null()) {
            return std::string{};
        }
#if ZBB_CONFIG_PROPERTIES_FORMAT_PRETTY
        return j.dump(4);
#else
        return j.dump();
#endif
    }

    static value_type decode(std::string const& s) {
        try { 
            auto j = nlohmann::json::parse(s);
            if (j.is_null()) {
                return value_type{};
            }
            return from_json(j);
        } catch (std::exception const& e) {
            std::cerr << "decode error: "
                      << e.what()
                      << std::endl;
            return value_type{};
        }
    }
};

template<typename K, typename ... Ts>
class properties {
public:
    using key_type = K;
    using codec_type = properties_codec<K, Ts ...>;
    using value_type = std::tuple<Ts ...>;
    using self_type = properties<K, Ts ...>;
public:
    properties() = default;
    properties(value_type const& value)
        : value_{value} {
        // EMPTY
    }
    properties(Ts const&& ... args)
        : properties{std::forward_as_tuple(args ...)} {
        // EMPTY
    }
    properties(Ts const& ... args)
        : properties{std::forward_as_tuple(args ...)} {
        // EMPTY
    }
 public:
    bool available () const noexcept {
        return value_type{} != value_;
    }
    operator bool () const noexcept {
        return available();
    }
public:
    value_type value() const {
        return value_;
    }
    properties& value(value_type const& value) {
        value_ = value;
        return *this;
    }
public:
    template<std::size_t I>
    std::tuple_element_t<I, value_type> get() const {
        return std::get<I>(value_);
    }
    template<std::size_t I>
    properties& set(std::tuple_element_t<I, value_type>&& e) {
        std::get<I>(value_) = e;
        return *this;
    }
    template<std::size_t I>
    properties& set(std::tuple_element_t<I, value_type> const& e) {
        std::get<I>(value_) = e;
        return *this;
    }
public:
    static std::string serialize(properties const& prop) noexcept {
        return codec_type::encode(prop.value());
    }
    static properties deserialize(std::string const& bytes) noexcept {
        return properties{codec_type::decode(bytes)};
    }
public:
    static properties from_json(::nlohmann::json const& j) noexcept {
        return properties{codec_type::from_json(j)};
    }
    static ::nlohmann::json to_json(properties const& prop) noexcept {
        return codec_type::to_json(prop.value());
    }
private:
    value_type value_;
};


////////////////////////////////////////////////////////////////////////////////


struct properties_base {
public:
    virtual operator bool () const noexcept = 0;
    virtual void reset() noexcept = 0;
protected:
    properties_base() = default;
    ~properties_base() = default;
};

template<typename ClaazT, typename FieldT>
struct filed {
    using self_type = filed<ClaazT, FieldT>;
    using clazz_type = std::decay_t<ClaazT>;
    using type = FieldT;
    using pointer_type = type clazz_type::*;

    pointer_type pointer = nullptr;
    const char* name = nullptr;

    filed() = default;
    ~filed() = default;

    constexpr filed( pointer_type const& initial_pointer
                   , char const* initial_name)
        : pointer(initial_pointer)
        , name(initial_name) {
        // EMPTY
    }

    constexpr filed( pointer_type const&& initial_pointer
                   , char const* initial_name)
        : pointer(initial_pointer)
        , name(initial_name) {
        // EMPTY
    }
};

template<typename T, typename F>
using field_pointer_t = typename filed<T, F>::pointer_type;

template<typename T>
struct from_json_predicate {
    from_json_predicate(T& obj, nlohmann::json const& j)
        : obj_(obj)
        , j_(j) {
        // EMPTY
    }
    template<std::size_t I, typename E>
    bool apply(E& element) {
        try {
            using field_type = typename E::type;
            auto const& field_key = element.name;
            auto& field_variable = (obj_.*(element.pointer));
            field_variable = j_.at(field_key). template get<field_type>();
            return true;
        } catch (std::exception const& e) {
            std::cerr << "from_json_predicate error: "
                        << e.what()
                        << std::endl;
            return false;
        }
        return false;
    }
private:
    T& obj_;
    nlohmann::json const& j_;
};

template<typename T>
struct to_json_predicate {
    explicit to_json_predicate(nlohmann::json& j, T const& obj)
        : j_(j)
        , obj_(obj) {
        // EMPTY
    }
    template<std::size_t I, typename E>
    bool apply(E const& element) {
        try {
            auto const& field_key = element.name;
            auto const& field_variable = obj_.*(element.pointer);
            j_[field_key] = field_variable;
            return true;
        } catch (std::exception const& e) {
            std::cerr << "to_json_predicate error: "
                        << e.what()
                        << std::endl;
            return false;
        }
    }
private:
    ::nlohmann::json& j_;
    T const& obj_;
};


////////////////////////////////////////////////////////////////////////////////


template<typename K, typename ... Ts>
static void from_json(::nlohmann::json const& j, properties<K, Ts ...>& p) {
    p = properties<K, Ts ...>::from_json(j);
}

template<typename K, typename ... Ts>
static void to_json(::nlohmann::json& j, properties<K, Ts ...> const& p) {
    j = properties<K, Ts ...>::to_json(p);
}

template<typename K, typename ... Ts>
static std::string serialize(properties<K, Ts ...> const& p) {
    return properties<K, Ts ...>::serialize(p);
}

template< typename K, typename ... Ts>
static properties<K, Ts ...> deserialize(std::string const& s) {
    return properties<K, Ts ...>::deserialize(s);
}


////////////////////////////////////////////////////////////////////////////////


template< typename T
        , typename = std::enable_if_t<std::is_base_of<base, T>::value>>
static void from_json(::nlohmann::json const& j, T& obj) {
    auto const meta = T::meta();
    using meta_type = decltype(meta);
    ::zbb::util::static_iterate(meta, from_json_predicate<T>{obj, j});
}

template< typename T
        , typename = std::enable_if_t<std::is_base_of<base, T>::value>>
static void to_json(::nlohmann::json& j, T const& obj) {
    auto const meta = T::meta();
    using meta_type = decltype(meta);
    ::zbb::util::static_iterate(meta, to_json_predicate<T>{j, obj});
}

template< typename T
        , typename = std::enable_if_t<std::is_base_of<base, T>::value>>
static std::string serialize(T const& obj) {
    ::nlohmann::json j;
    to_json(j, obj);
    if (j.is_null()) {
        return std::string{};
    }
#if ZBB_CONFIG_PROPERTIES_FORMAT_PRETTY
    return j.dump(4);
#else
    return j.dump();
#endif
}

template< typename T
        , typename = std::enable_if_t<std::is_base_of<base, T>::value>>
static T deserialize(std::string const& s) {
    try { 
        auto j = nlohmann::json::parse(s);
        if (j.is_null()) {
            return T{};
        }
        T obj;
        from_json(j, obj);
        return obj;
    } catch (std::exception const& e) {
        std::cerr << "decode error: "
                    << e.what()
                    << std::endl;
        return T{};
    }
}

ZBB_CONFIG_END


#endif // ZBB_CONFIG_PROPERTIES_HH

// JsonSerialization.h

#pragma once

#include <json/json.h>
#include <cassert>
#include <type_traits>

namespace Json {
    // Forward declarations for serialization and deserialization
    //These are needed so that each class that uses this can set these 2 functions as friends before the 
    //definitions of the functions are defined
    //template<typename T>
    //void serialize(Json::Value& value, const T& o);

    //template<typename T>
    //void deserialize(const Json::Value& value, T& o);

    // Template specialization for serialization and deserialization
    template<typename T>
    void serialize(Json::Value& value, T& o) {
        static_assert(std::is_class<T>::value, "Json serialization is only supported for classes.");
        // Default serialization implementation
        // You can provide a generic serialization logic here if needed
    }

    template<typename T>
    void deserialize(Json::Value& value, T& o) {
        static_assert(std::is_class<T>::value, "Json deserialization is only supported for classes.");
        // Default deserialization implementation
        // You can provide a generic deserialization logic here if needed
    }
}


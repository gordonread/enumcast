#pragma once

#include <iostream>
#include <regex>
#include <utility>
#include <vector>
#include <unordered_map>
#include <memory>

namespace EnumCast
{
namespace Detail
{
template <auto... Values>
constexpr inline auto getEnumeratorNamesImpl()
{
    return __PRETTY_FUNCTION__;
}

template <typename Enum, auto... Values>
constexpr inline auto getEnumeratorNames(std::integer_sequence<int32_t, Values...>)
{
    return getEnumeratorNamesImpl<static_cast<Enum>(Values)...>();
}

template <typename Enum, typename = decltype(Enum::UnknownValue)>
constexpr inline int32_t unknownValue(int)
{
    return static_cast<int32_t>(Enum::UnknownValue);
}
template <typename Enum>
constexpr inline int32_t unknownValue(...)
{
    return static_cast<int32_t>(std::numeric_limits<std::underlying_type_t<Enum>>::min());
}

template <typename Enum, typename = decltype(Enum::MinValue)>
constexpr inline int32_t minValue(int)
{
    return static_cast<int32_t>(Enum::MinValue);
}
template <typename Enum>
constexpr inline int32_t minValue(...)
{
    return static_cast<int32_t>(std::numeric_limits<std::underlying_type_t<Enum>>::min());
}

template <typename Enum, typename = decltype(Enum::MaxValue)>
constexpr inline int32_t maxValue(int)
{
    return static_cast<int32_t>(Enum::MaxValue);
}
template <typename Enum>
constexpr inline int32_t maxValue(...)
{
    return static_cast<int32_t>(std::numeric_limits<std::underlying_type_t<Enum>>::max());
}

template <typename Enum>
struct Value
{
    static constexpr inline int32_t unknown {unknownValue<Enum>(0)};
    static constexpr inline int32_t min {minValue<Enum>(0)};
    static constexpr inline int32_t max {maxValue<Enum>(0)};
};

template <int32_t N, typename Seq>
struct OffsetSequence;

template <int32_t N, int32_t... Ints>
struct OffsetSequence<N, std::integer_sequence<int32_t, Ints...>>
{
    using type = std::integer_sequence<int32_t, Ints + N...>;
};

template <int32_t N, typename Seq>
using OffsetSequenceT = typename OffsetSequence<N, Seq>::type;

template <typename Enum>
constexpr inline auto enumNames()
{
    return std::regex_replace(
        getEnumeratorNames<Enum>(
            OffsetSequenceT<Value<Enum>::min,
                            std::make_integer_sequence<int32_t, static_cast<std::size_t>(Value<Enum>::max + 1 -
                                                                                         Value<Enum>::min)>> {}),
        std::regex("^.*[{]|[(][^)]*[)]|,|[}].*$"), "");
}

template <typename Enum>
constexpr inline auto toStringVec()
{
    const auto s {enumNames<Enum>()};
    const std::regex re {R"(\s+)"};
    const std::vector<std::string> result(std::sregex_token_iterator {s.begin(), s.end(), re, -1},
                                          std::sregex_token_iterator {});
    return result;
}
}

template <typename Enum>
constexpr inline std::string toString(Enum value)
{
    const auto qualifiedString {
        ":" + EnumCast::Detail::toStringVec<Enum>()[static_cast<int32_t>(value) - EnumCast::Detail::Value<Enum>::min]};
    return qualifiedString.substr(qualifiedString.find_last_of(":") + 1);
}

template <typename Enum>
constexpr inline std::string toQualifiedString(Enum value)
{
    return EnumCast::Detail::toStringVec<Enum>()[static_cast<int32_t>(value) - EnumCast::Detail::Value<Enum>::min];
}

template <typename Enum>
inline Enum toEnum(const std::string& str)
{
    static std::unordered_map<std::string, Enum> data {};
    static bool first {true};
    if (first)
    {
        first = false;
        const std::regex re {R"(^[0-9]+$)"};
        size_t idx {0};
        for (const auto& name : EnumCast::Detail::toStringVec<Enum>())
        {
            std::string shortName {(":" + name).substr((":" + name).find_last_of(":") + 1)};
            if (!std::regex_match(shortName, re))
            {
                data.emplace(shortName, static_cast<Enum>(idx + EnumCast::Detail::Value<Enum>::min));
            }
            ++idx;
        }
    }
    const auto iter {data.find(str)};
    return iter == data.cend() ? static_cast<Enum>(EnumCast::Detail::Value<Enum>::unknown) : iter->second;
}

template <typename Enum>
constexpr inline Enum toEnum(const char* str)
{
    return toEnum<Enum>(std::string {str});
}

template <typename Enum>
inline Enum toQualifiedEnum(const std::string& str)
{
    static std::unordered_map<std::string, Enum> data {};
    static bool first {true};
    if (first)
    {
        first = false;
        const std::regex re {R"(^[0-9]+$)"};
        size_t idx {0};
        for (const auto& name : EnumCast::Detail::toStringVec<Enum>())
        {
            if (!std::regex_match(name, re))
            {
                data.emplace(name, static_cast<Enum>(idx + EnumCast::Detail::Value<Enum>::min));
            }
            ++idx;
        }
    }
    const auto iter {data.find(str)};
    return iter == data.cend() ? static_cast<Enum>(EnumCast::Detail::Value<Enum>::unknown) : iter->second;
}

template <typename Enum>
constexpr inline Enum toQualifiedEnum(const char* str)
{
    return toQualifiedEnum<Enum>(std::string {str});
}
}

template <typename T, typename U>
constexpr inline T enum_cast(const U& u)
{
    if constexpr (std::is_enum_v<U>)
    {
        return EnumCast::toString(u);
    }
    else
    {
        return EnumCast::toEnum<T>(u);
    }
}

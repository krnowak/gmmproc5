#include <iostream>
#include <tuple>

namespace API
{

    struct Stuff {};

    template <typename TagP, typename... TypeP>
    auto
    tuple (TagP, TypeP...)
    {
        return std::tuple<Stuff, TypeP...> {};
    }

}

namespace Foo
{

    struct Tag {};

    struct Filler {};

    template <typename... TypeP>
    auto
    tuple (Tag, TypeP...)
    {
        return std::tuple<Filler, TypeP...> {};
    }

}

namespace Bar
{

    struct Type1 {};
    struct Type2 {};
    struct Type3 {};

}

struct GenericTag {};

struct Test {};

void
check_type (Test, Test)
{}

int main()
{
    using API::tuple;

    GenericTag gt {};
    Foo::Tag ft {};
    Bar::Type1 bt1;
    Bar::Type2 bt2;
    Bar::Type3 bt3;

    auto gt_tuple = tuple(gt, bt1, bt2, bt3);
    auto ft_tuple = tuple(ft, bt1, bt2, bt3);
    auto gt_first = std::get<0> (gt_tuple);
    auto ft_first = std::get<0> (ft_tuple);
    auto gt_second = std::get<1> (gt_tuple);
    auto ft_second = std::get<1> (ft_tuple);

    static_assert (std::is_same<API::Stuff, decltype(gt_first)>::value, "");
    static_assert (std::is_same<Foo::Filler, decltype(ft_first)>::value, "");
    static_assert (std::is_same<Bar::Type1, decltype(gt_second)>::value, "");
    static_assert (std::is_same<Bar::Type1, decltype(ft_second)>::value, "");

    check_type (gt_tuple, ft_tuple);
}

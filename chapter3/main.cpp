#include <iostream>
#include <type_traits>
#include <vector>

using std::cout;
using std::endl;

template <typename T> struct as_constref { using type = T const&; };

using cref = as_constref<float>::type;

template <class List> struct size;

template <template <class...> class List, class... Elements>
struct size<List<Elements...>>
    : std::integral_constant<std::size_t, sizeof...(Elements)> {};

template <class List, class New> struct push_back;

template <template <class...> class List, class... Elements, class New>
struct push_back<List<Elements...>, New> {
    using type = List<Elements..., New>;
};

template <class List> struct remove_front;

template <template <class...> class List, class Head, class... Elements>
struct remove_front<List<Head, Elements...>> {
    using type = List<Elements...>;
};

template <template <class...> class List> struct remove_front<List<>> {
    using type = List<>;
};

template <class List> struct as_tuple;
template <template <class...> class List, class... Elements>
struct as_tuple<List<Elements...>> {
    using type = std::tuple<Elements...>;
};

template <class Container, class List> struct recontainer;

template <template <class...> class Container, template <class...> class List,
          class... Elements>
struct recontainer<Container<>, List<Elements...>> {
    using type = Container<Elements...>;
};

template <typename L1, typename L2> struct append;

template <template <class...> class L1, typename... T1,
          template <class...> class L2, typename... T2>
struct append<L1<T1...>, L2<T2...>> {
    using type = L1<T1..., T2...>;
};

struct remove_ptr {
    template <typename T> struct apply {
        using type = typename std::remove_pointer_t<T>;
    };
};

int main() {
    constexpr auto s = size<std::tuple<int, float, void>>::value;
    cout << "size<std::tuple<int, float, void>>::value is " << s << endl;

    using no_ptr = remove_ptr::apply<int*>::type;

    return 0;
}
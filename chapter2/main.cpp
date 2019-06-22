#include <iostream>
#include <random>
#include <tuple>
#include <type_traits>

using std::cout;
using std::endl;

using Location = float;
using Time = float;

// double_ptr_type will be double*
using double_ptr_type = std::add_pointer_t<double>;
using double_type = std::remove_pointer_t<double*>;
using double_too_type = std::remove_pointer_t<double>;

// Here we're pretending that adjust_values is some legacy code, and there
// is a lot more of it. We can't change the interface, and the code itself
// has been battle-tested.
void adjust_values(double* alpha1, double* beta1, double* alpha2,
                   double* beta2) {
    cout << "adjust_values called with:" << endl;
    cout << "alpha1 = " << *alpha1 << endl;
    cout << "beta1  = " << *beta1 << endl;
    cout << "alpha2 = " << *alpha2 << endl;
    cout << "beta2  = " << *beta2 << endl;
}

class Reading {
  public:
    Reading() : rng(std::random_device()()) {}
    double alpha_value(Location l, Time t) {
        std::uniform_real_distribution<> distribution(3.0, 6.0);
        return distribution(rng) * l * t;
    }
    double beta_value(Location l, Time t) {
        std::uniform_real_distribution<> distribution(42.0, 43.0);
        return distribution(rng) * l * t;
    }

  private:
    std::mt19937 rng;
};

// A template function that chains a call to a value function followed by a
// call to a pointer function.
template <typename ValueFunction, typename PointerFunction>
double magic_wand(ValueFunction vf, PointerFunction pf, double param) {
    double v = vf(param);
    pf(&v);
    return v;
}

// Template metaprogramming to take a function F, and return a tuple of the
// function parameters.
// ****************************************************************************
template <typename F> struct make_tuple_of_params;

template <typename Ret, typename... Args>
struct make_tuple_of_params<Ret(Args...)> {
    using type = std::tuple<Args...>;
};

// Convenience Function
template <typename F>
using make_tuple_of_params_t = typename make_tuple_of_params<F>::type;
// ****************************************************************************

// Template metaprogramming to take a function F, and return a tuple of the
// dereferenced function parameters.
// ****************************************************************************
template <typename F> struct make_tuple_of_derefed_params;

template <typename Ret, typename... Args>
struct make_tuple_of_derefed_params<Ret(Args...)> {
    using type = std::tuple<std::remove_pointer_t<Args>...>;
};

// Convenience Function
template <typename F>
using make_tuple_of_derefed_params_t =
    typename make_tuple_of_derefed_params<F>::type;
// ****************************************************************************

template <std::size_t FunctionIndex, typename FunctionsTuple, typename Params,
          std::size_t... I>
auto dispatch_params(FunctionsTuple& functions, Params& params,
                     std::index_sequence<I...>) {
    return (std::get<FunctionIndex>(functions))(std::get<I>(params)...);
}

template <typename FunctionsTuple, std::size_t... I, typename Params,
          typename ParamsSeq>
auto dispatch_functions(FunctionsTuple& functions, std::index_sequence<I...>,
                        Params& params, ParamsSeq params_seq) {
    return std::make_tuple(
        dispatch_params<I>(functions, params, params_seq)...);
}

template <typename F, typename Tuple, std::size_t... I>
void dispatch_to_c(F f, Tuple& t, std::index_sequence<I...>) {
    f(&std::get<I>(t)...);
}

template <typename LegacyFunction, typename... Functions, typename... Params>
auto magic_wand(LegacyFunction legacy,
                const std::tuple<Functions...>& functions,
                const std::tuple<Params...>& params1,
                const std::tuple<Params...>& params2) {
    static const std::size_t functions_count = sizeof...(Functions);
    static const std::size_t params_count = sizeof...(Params);
    auto params = std::tuple_cat(
        dispatch_functions(functions,
                           std::make_index_sequence<functions_count>(), params1,
                           std::make_index_sequence<params_count>()),
        dispatch_functions(functions,
                           std::make_index_sequence<functions_count>(), params2,
                           std::make_index_sequence<params_count>()));

    static constexpr auto t_count = std::tuple_size<decltype(params)>::value;

    dispatch_to_c(legacy, params, std::make_index_sequence<t_count>());
    return params;
}

template <typename R>
std::tuple<double, double, double, double>
get_adjusted_values(R& r, Location l, Time t1, Time t2) {
    return magic_wand(
        adjust_values,
        std::make_tuple([&r](int l, int t) { return r.alpha_value(l, t); },
                        [&r](int l, int t) { return r.beta_value(l, t); }),
        std::make_tuple(l, t1), std::make_tuple(l, t2));
}

int main() {
    auto r = Reading();
    get_adjusted_values(r, 1, 2, 3);

    return 0;
}
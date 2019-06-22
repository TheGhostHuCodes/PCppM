#include <cstdint>
#include <iostream>
#include <type_traits>

using std::cout;
using std::endl;

int main() {
    using T = short;
    // Assert type T is std::uint32_t.
    static_assert(std::is_same<T, std::uint32_t>::value,
                  "Invalid integer detected!");

    // Assert type T is of size 4 bytes.
    static_assert(sizeof(T) == 4, "Invalid integer size detected!");

    // Assert that we're on a 64-bit platform.
    static_assert(sizeof(void*) == 8, "Expected 64-bit platform.");

    static const std::uint64_t default_buffer_size = std::conditional<
        sizeof(void*) == 8,
        std::integral_constant<std::uint64_t, 1024 * 1024 * 1024>,
        std::integral_constant<std::uint64_t, 100 * 1024 * 1024>>::type::value;

    cout << "default_buffer_size: " << default_buffer_size << endl;

    return 0;
}
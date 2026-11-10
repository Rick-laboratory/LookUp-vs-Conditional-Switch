#include <array>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <string_view>
#include <system_error>

namespace {
constexpr std::array<std::uint32_t, 11> values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
constexpr std::size_t input_count = 65536;

std::uint32_t switch_value(std::uint8_t input) noexcept {
    switch (input) {
    case 0: return 0;
    case 1: return 1;
    case 2: return 2;
    case 3: return 3;
    case 4: return 4;
    case 5: return 5;
    case 6: return 6;
    case 7: return 7;
    case 8: return 8;
    case 9: return 9;
    case 10: return 10;
    default: return 0;
    }
}

std::uint32_t lookup_value(std::uint8_t input) noexcept {
    return input < values.size() ? values[input] : 0;
}

struct Measurement {
    std::uint64_t checksum;
    double milliseconds;
};

template <typename Dispatch>
Measurement measure(const std::array<std::uint8_t, input_count>& inputs,
                    std::uint32_t rounds, Dispatch dispatch) {
    // Observable input reads stop the compiler from folding the entire loop away.
    // Their cost is shared by both measurements.
    const volatile std::uint8_t* data = inputs.data();
    std::uint64_t checksum = 0;
    const auto start = std::chrono::steady_clock::now();
    for (std::uint32_t round = 0; round < rounds; ++round) {
        for (std::size_t index = 0; index < inputs.size(); ++index) {
            checksum += dispatch(data[index]);
        }
    }
    const auto end = std::chrono::steady_clock::now();
    return {checksum, std::chrono::duration<double, std::milli>(end - start).count()};
}

void print_measurement(std::string_view label, const Measurement& result) {
    std::cout << std::left << std::setw(14) << label
              << std::fixed << std::setprecision(3) << result.milliseconds
              << " ms  checksum=" << result.checksum << '\n';
}
} // namespace

int main(int argc, char* argv[]) {
    if (argc == 2 && (std::string_view(argv[1]) == "--help" ||
                      std::string_view(argv[1]) == "-h")) {
        std::cout << "Usage: dispatch-benchmark [rounds]\n"
                     "Rounds: 1..100000 (default 100). Use an optimized build.\n";
        return 0;
    }

    std::uint32_t rounds = 100;
    if (argc == 2) {
        const std::string_view argument = argv[1];
        const auto parsed = std::from_chars(argument.data(),
                                            argument.data() + argument.size(), rounds);
        if (parsed.ec != std::errc{} || parsed.ptr != argument.data() + argument.size() ||
            rounds == 0 || rounds > 100000) {
            std::cerr << "Rounds must be an integer between 1 and 100000.\n";
            return 2;
        }
    } else if (argc > 2) {
        std::cerr << "Usage: dispatch-benchmark [rounds]\n";
        return 2;
    }

    std::array<std::uint8_t, input_count> inputs{};
    std::mt19937 random{42};
    std::uint64_t expected_per_round = 0;
    for (auto& value : inputs) {
        value = static_cast<std::uint8_t>(random() % 16);
        // The specified mapping is the identity on 0..10, and zero otherwise.
        expected_per_round += value <= 10 ? value : 0;
    }

    const Measurement conditional = measure(inputs, rounds, switch_value);
    const Measurement table = measure(inputs, rounds, lookup_value);
    const std::uint64_t expected = expected_per_round * rounds;
    if (conditional.checksum != expected || table.checksum != expected) {
        std::cerr << "The dispatch implementations produced an incorrect checksum.\n";
        return 1;
    }

    std::cout << static_cast<std::uint64_t>(rounds) * inputs.size()
              << " inputs per method; fixed seed 42\n";
    print_measurement("Switch", conditional);
    print_measurement("Lookup table", table);
    std::cout << "These timings describe this build and workload, not a general rule.\n";
    return 0;
}

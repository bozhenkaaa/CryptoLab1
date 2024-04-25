#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <cassert>
#include <bitset>
#include <string>
#include <sstream>
#include <iomanip>
#include "base64_default_rfc4648.hpp"

std::string HexToBase64(const std::string& hex_str) {
    std::string bytes;
    for (size_t i = 0; i < hex_str.size(); i += 2) {
        std::string byte_str = hex_str.substr(i, 2);
        char byte = static_cast<char>(std::strtol(byte_str.c_str(), nullptr, 16));
        bytes.push_back(byte);
    }
    std::string base64_str = cppcodec::base64_rfc4648::encode(bytes);
    return base64_str;
}

long long mod_pow(long long base, long long exponent, long long modulus) {
    long long result = 1;
    base = base % modulus;
    while (exponent > 0) {
        if ((exponent % 2) == 1)
            result = (result * base) % modulus;
        base = (base * base) % modulus;
        exponent /= 2;
    }
    return result;
}

bool millerTest(long long d, long long n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long long> dis(2, n - 4);
    long long a = dis(gen);
    long long x = mod_pow(a, d, n);
    if (x == 1 || x == n - 1)
        return true;
    while (d != n - 1) {
        x = (x * x) % n;
        d *= 2;
        if (x == 1)
            return false;
        if (x == n - 1)
            return true;
    }
    return false;
}

bool isPrimeRM(long long n, int k) {
    if (n <= 1 || n == 4)
        return false;
    if (n <= 3)
        return true;
    long long d = n - 1;
    while (d % 2 == 0)
        d /= 2;
    for (int i = 0; i < k; i++)
        if (!millerTest(d, n))
            return false;
    return true;
}

int jacobi_symbol(int a, int n) {
    assert(n > 0 && n % 2 == 1);

    int t = 1;
    while (a != 0) {
        while (a % 2 == 0) {
            a /= 2;
            int r = n % 8;
            if (r == 3 || r == 5) {
                t = -t;
            }
        }
        std::swap(a, n);
        if (a % 4 == 3 && n % 4 == 3) {
            t = -t;
        }
        a %= n;
    }
    return n == 1 ? t : 0;
}

std::vector<long long> prime_factors(long long n) {
    std::vector<long long> factors;
    // Check for number of 2s that divide n.
    while (n % 2 == 0) {
        factors.push_back(2);
        n /= 2;
    }
    // n must be odd at this point, thus a skip of 2 (i.e., i = i + 2) can be used
    for (long long i = 3; i <= sqrt(n); i += 2) {
        // While i divides n, append i and divide n
        while (n % i == 0) {
            factors.push_back(i);
            n /= i;
        }
    }
    // This condition is to handle the case when n is a prime number
    // greater than 2
    if (n > 2)
        factors.push_back(n);
    return factors;
}

bool lucas_test(long long n) {
    if (n == 1) {
        return false;
    }
    if (n == 2) {
        return true;
    }
    if (n % 2 == 0) {
        return false;
    }

    // Factorize n-1
    std::vector<long long> factors = prime_factors(n - 1);
    int trial_count = 10;  // Number of trials

    for (int _ = 0; _ < trial_count; _++) {
        long long a = rand() % (n - 2) + 2;
        if (mod_pow(a, n - 1, n) != 1) {
            return false;  // n is not prime
        }
        bool flag = true;
        for (long long factor : factors) {
            if (mod_pow(a, (n - 1) / factor, n) == 1) {
                flag = false;
                break;
            }
        }
        if (flag) {
            return true;  // n is probably prime
        }
    }
    return false;  // n is not prime if it fails all tests
}
bool baillie_test(long long num) {
    std::vector<long long> known_primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
    for (long long known_prime : known_primes) {
        if (num == known_prime) {
            return true;
        } else if (num % known_prime == 0) {
            return false;
        }
    }
    if (!isPrimeRM(num, 2)) {
        return false;
    }
    if (!lucas_test(num)) {
        return false;
    }
    return true;
}
long long generate_prime(int bits) {
    assert(bits > 1);  // The number must be at least 2 bits
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long long> dis(1LL << (bits - 1), (1LL << bits) - 1);
    while (true) {
        // Generate a random number of 'bits' bits
        long long candidate = dis(gen);
        // Set the most significant bit and the least significant bit to 1 to ensure bitness and oddness
        candidate |= (1LL << (bits - 1)) | 1;
        if (isPrimeRM(candidate, 50)) {
            return candidate;
        }
    }
}

std::string convert_output(long long value, const std::string& output_format) {
    if (output_format == "base2") {
        std::bitset<64> binary(value);
        return binary.to_string();
    } else if (output_format == "base10") {
        return std::to_string(value);
    } else if (output_format == "base64") {
        std::stringstream ss;
        ss << std::setw(8) << std::setfill('0') << std::hex << value;
        std::string hex_str = ss.str();
        std::string base64_str = HexToBase64(hex_str);
        return base64_str;
    } else if (output_format == "byte[]") {
        std::stringstream ss;
        for (int i = 7; i >= 0; --i) {
            ss << ((value >> (i * 8)) & 0xFF) << " ";
        }
        return ss.str();
    } else {
        return "Invalid format";
    }
}

int main() {
    while (true) {
        std::cout << "\nMenu:\n";
        std::cout << "1. Find a prime number with a specified number of bits.\n";
        std::cout << "2. Check a specific number for simplicity (Baillie-PSW).\n";
        std::cout << "3. Check a specific number for simplicity (Miller-Rabin).\n";
        std::cout << "4. Quick exponentiation modulo.\n";
        std::cout << "5. Exit.\n";

        int choice;
        std::cout << "Enter your choice (1-5): ";
        std::cin >> choice;
        if (choice == 1) {
            int bits;
            std::cout << "Enter the number of bits for the prime number: ";
            std::cin >> bits;
            long long prime_number = generate_prime(bits);
            std::cout << "Generated prime number: " << prime_number << "\n";
        } else if (choice == 2) {
            long long number;
            std::cout << "Enter the number to check for simplicity: ";
            std::cin >> number;
            if (baillie_test(number)) {
                std::cout << "Number " << number << " is prime.\n";
            } else {
                std::cout << "Number " << number << " is not prime.\n";
            }
        } else if (choice == 3) {
            long long number;
            int k;
            std::cout << "Enter the number to check for simplicity: ";
            std::cin >> number;
            std::cout << "Enter the number of test iterations: ";
            std::cin >> k;
            if (isPrimeRM(number, k)) {
                std::cout << "Number " << number << " is prime.\n";
            } else {
                std::cout << "Number " << number << " is not prime.\n";
            }
        } else if (choice == 4) {
            long long base, exponent, modulus;
            std::cout << "Enter base: ";
            std::cin >> base;
            std::cout << "Enter exponent: ";
            std::cin >> exponent;
            std::cout << "Enter modulus: ";
            std::cin >> modulus;
            long long result = mod_pow(base, exponent, modulus);
            std::string format_choice;
            std::cout << "Choose output format (base2, base10, base64, byte[]): ";
            std::cin >> format_choice;
            std::string formatted_result = convert_output(result, format_choice);
            std::cout << "Result " << base << "^" << exponent << " mod " << modulus << " = " << formatted_result << "\n";
        } else if (choice == 5) {
            std::cout << "Exiting program.\n";
            break;
        } else {
            std::cout << "Invalid choice, try again.\n";
        }
    }

    return 0;
}
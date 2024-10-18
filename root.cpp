#include <iostream>
#include <cstring>
#include <cmath>


class RSA {
public:
    RSA(int p, int q) {
        n = p * q;  // modulus
        phi = (p - 1) * (q - 1);

        e = 3;  // Small odd number for e, co-prime with phi
        while (gcd(e, phi) != 1) {
            e += 2;  // Find next odd number if e and phi are not co-prime
        }

        d = modInverse(e, phi);  // Calculate the private key d
    }

    int encrypt(int message) {
        return modExp(message, e, n);  // Ciphertext = (message^e) % n
    }

    int decrypt(int ciphertext) {
        return modExp(ciphertext, d, n);  // Plaintext = (ciphertext^d) % n
    }

private:
    int n, phi, e, d;

    // Euclidean Algorithm to find GCD
    int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }

    // Extended Euclidean Algorithm to find modular inverse
    int modInverse(int a, int m) {
        int m0 = m, t, q;
        int x0 = 0, x1 = 1;

        if (m == 1)
            return 0;

        while (a > 1) {
            q = a / m;
            t = m;

            m = a % m;
            a = t;
            t = x0;

            x0 = x1 - q * x0;
            x1 = t;
        }

        if (x1 < 0)
            x1 += m0;

        return x1;
    }

    // Modular exponentiation (base^exp % mod)
    int modExp(int base, int exp, int mod) {
        int result = 1;
        base = base % mod;
        while (exp > 0) {
            if (exp % 2 == 1) {
                result = (result * base) % mod;
            }
            exp = exp >> 1;
            base = (base * base) % mod;
        }
        return result;
    }
};

int main() {
   
    int p = 61, q = 53;  // Two large prime numbers
    RSA rsa(p, q);

    int message = 65;  // Example message
    int encryptedMessage = rsa.encrypt(message);
    int decryptedMessage = rsa.decrypt(encryptedMessage);

    std::cout << "RSA Encrypted: " << encryptedMessage << std::endl;
    std::cout << "RSA Decrypted: " << decryptedMessage << std::endl;

    return 0;
}
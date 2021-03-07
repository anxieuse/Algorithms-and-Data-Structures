#include <complex>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

enum TBigInt_exception {
    OUT_OF_RANGE,
    SUBTRACTION_ERROR,
    DIV_BY_ZERO,
    EXPONENTIATION_TO_ZERO
};

const int BASE = 1e5;
const int RADIX_LEN = 5;

typedef std::complex<long double> base;
const long double PI = acos(-1);

void fourierTransform(std::vector<base> &a, bool invert) {
    int n = a.size();

    /* end of recursion: return */
    if (n == 1)
        return;

    /* split a into two polynoms of size n/2 */
    std::vector<base> a0(n / 2);
    std::vector<base> a1(n / 2);
    for (int i = 0; i < n; i += 2) {
        a0[i / 2] = a[i];
        a1[i / 2] = a[i + 1];
    }

    /* recursive-call */
    fourierTransform(a0, invert);
    fourierTransform(a1, invert);

    /* angle between im. roots */
    long double ang = (2 * PI / n) * (invert ? -1 : 1);

    base w(1);
    base wn(cos(ang), sin(ang));
    for (int i = 0; i < n / 2; ++i) {
        /* formula: a_(0...n/2 - 1) = a_0 + x * a_1 */
        a[i] = a0[i] + w * a1[i];
        /* formula: a_(n/2...n) = a_0 - x * a_1 */
        a[i + n / 2] = a0[i] - w * a1[i];
        if (invert) {
            a[i] /= 2;
            a[i + n / 2] /= 2;
        }
        w *= wn;
    }
}

void multiply(const std::vector<int> &a,
              const std::vector<int> &b,
              std::vector<long long> &res, int n, int m) {
    /* polynom with integer-coeffs to imaginary */
    std::vector<base> complexA(begin(a), end(a));
    std::vector<base> complexB(begin(b), end(b));

    /* newSize is max(m,n) upper bound, which is a power of two */
    int newSize = 1;
    while (newSize < n or newSize < m) {
        newSize *= 2;
    }
    newSize *= 2;

    /* resize complA and compB filling it with zero-coefficients */
    complexA.resize(newSize);
    complexB.resize(newSize);

    /* transform coefficients to values of polynoms in each of (2pi/n) im. points  */
    fourierTransform(complexA, false);
    fourierTransform(complexB, false);

    /* as known, res(x) value eqs to a(x) * b(x), so lets calculate it, but using complexA as temporary container: */
    for (int i = 0; i < newSize; ++i) {
        complexA[i] = complexA[i] * complexB[i];
    }

    /* finally, get complex coefficients */
    fourierTransform(complexA, true);

    /* and transform it to real... */
    res.resize(newSize);
    for (int i = 0; i < newSize; ++i) {
        /* ...flooring it's value because of погрешность */
        res[i] = round(complexA[i].real());
    }

    int carry = 0;
    for (int i = 0; i < newSize; ++i) {
        res[i] += carry;
        carry = res[i] / BASE;
        res[i] %= BASE;
    }
}

struct TBigInt {
    std::vector<int> digits;

    TBigInt() = default;
    explicit TBigInt(const std::string &num_string) {
        this->Initialize(num_string);
    }

    void Initialize(const std::string &num_string) {
        this->digits.clear();
        int num_len = num_string.size();
        for (int radix_end = num_len - 1; radix_end >= 0; radix_end -= RADIX_LEN) {
            if (radix_end >= RADIX_LEN - 1) {
                digits.emplace_back(std::stoll(num_string.substr(radix_end - RADIX_LEN + 1, RADIX_LEN)));
            } else {
                digits.emplace_back(std::stoll(num_string.substr(0, radix_end + 1)));
            }
        }
        this->RemoveLeadingZeros();
    }

    friend std::ostream &operator<<(std::ostream &out, const TBigInt &rhs) {
        int num_len = rhs.digits.size();
        out << rhs.digits.back();
        for (int radix = num_len - 2; radix >= 0; --radix) {
            out << std::setfill('0') << std::setw(RADIX_LEN) << rhs.digits[radix];
        }
        return out;
    }

    friend std::istream &operator>>(std::istream &in, TBigInt &rhs) {
        std::string num_string;
        in >> num_string;
        rhs.Initialize(num_string);
        return in;
    }

    int Compare(const TBigInt &rhs) const {
        int n = this->digits.size();
        int m = rhs.digits.size();

        if (n < m) {
            return -1;
        } else if (n > m) {
            return 1;
        }

        for (int i = n - 1; i >= 0; --i) {
            if (this->digits[i] < rhs.digits[i]) {
                return -1;
            } else if (this->digits[i] > rhs.digits[i]) {
                return 1;
            }
        }

        return 0;
    }

    bool operator<(const TBigInt &rhs) {
        return (this->Compare(rhs) == -1);
    }

    bool operator<=(const TBigInt &rhs) {
        return (this->Compare(rhs) <= 0);
    }

    bool operator>(const TBigInt &rhs) {
        return (this->Compare(rhs) == 1);
    }

    bool operator>=(const TBigInt &rhs) {
        return (this->Compare(rhs) >= 0);
    }

    bool operator==(const TBigInt &rhs) {
        return (this->Compare(rhs) == 0);
    }

    TBigInt operator+=(const TBigInt &rhs) {
        int carry = 0;
        for (int radix = 0; radix < std::max(this->digits.size(), rhs.digits.size()) || carry; ++radix) {
            if (radix == this->digits.size()) {
                this->digits.emplace_back(0);
            }
            this->digits[radix] += carry + (radix < rhs.digits.size() ? rhs.digits[radix] : 0);
            carry = this->digits[radix] / BASE;
            this->digits[radix] %= BASE;
        }
        return *this;
    }

    TBigInt operator-=(const TBigInt &rhs) {
        if (*this < rhs) {
            throw TBigInt_exception(SUBTRACTION_ERROR);
        }
        int carry = 0;
        for (int radix = 0; radix < static_cast<int>(rhs.digits.size()) || carry; ++radix) {
            this->digits[radix] -= carry + (radix < static_cast<int>(rhs.digits.size()) ? rhs.digits[radix] : 0);
            carry = this->digits[radix] < 0;
            if (carry != 0)
                this->digits[radix] += BASE;
        }
        this->RemoveLeadingZeros();
        return *this;
    }

    TBigInt operator*=(const TBigInt &rhs) {
        int n = this->digits.size();
        int m = rhs.digits.size();

        std::vector<long long> res;
        multiply(this->digits, rhs.digits, res, n, m); 

        this->digits = std::vector<int>(begin(res), end(res));
        this->RemoveLeadingZeros();
        return *this;
    }

    TBigInt operator*=(const int &rhs) {
        int carry = 0;
        for (int i = 0; i < static_cast<int>(this->digits.size()) || carry; ++i) {
            if (i == static_cast<int>(this->digits.size())) {
                this->digits.emplace_back(0);
            }
            long long cur = carry + 1ll * this->digits[i] * rhs;
            this->digits[i] = cur % BASE;
            carry = cur / BASE;
        }
        this->RemoveLeadingZeros();
        return *this;
    }

    TBigInt operator/=(const TBigInt &rhs) {
        if ((rhs.digits.size() == 1 && rhs.digits[0] == 0)) {
            throw TBigInt_exception(DIV_BY_ZERO);
        }
        if ((rhs.digits.size() == 1 && rhs.digits[0] == 1)) {
            return *this;
        }
        if (*this < rhs) {
            this->digits.assign(1, 0);
            return *this;
        }

        int n = this->digits.size();
        int quotient;
        TBigInt current;
        std::vector<int> res;
        for (int i = n - 1; i >= 0; --i) {
            current.digits.insert(begin(current.digits), this->digits[i]);
            if (current.digits.back() == 0)
                current.digits.pop_back();
            int l = -1, r = BASE, m;
            while (r - l > 1) {
                m = (r + l) / 2;
                if (rhs * m <= current) {
                    l = m;
                } else {
                    r = m;
                }
            }
            quotient = l;
            res.insert(begin(res), quotient);
            current -= rhs * quotient;
        }
        this->digits = res;
        this->RemoveLeadingZeros();
        return *this;
    }

    TBigInt operator/=(const int &rhs) {
        if (rhs == 0) {
            throw TBigInt_exception(SUBTRACTION_ERROR);
        }

        int n = this->digits.size();
        int carry = 0;
        for (int i = n - 1; i >= 0; --i) {
            long long cur = this->digits[i] + 1ll * carry * BASE;
            this->digits[i] = cur / rhs;
            carry = cur % rhs;
        }
        this->RemoveLeadingZeros();
        return *this;
    }

    TBigInt operator^=(TBigInt power) {
        if ((this->digits.size() == 1 && this->digits[0] == 0) and power == TBigInt("0")) {
            throw TBigInt_exception(EXPONENTIATION_TO_ZERO);
        }
        TBigInt base = *this;
        *this = TBigInt("1");
        while (!(power.digits.size() == 1 && power.digits[0] == 0)) {
            if (power.digits[0] % 2 == 0) {
                base *= base;
                power /= 2;
            } else {
                *this *= base;
                --power;
            }
        }
        this->RemoveLeadingZeros();
        return *this;
    }

    TBigInt operator=(const TBigInt &rhs) {
        this->digits = rhs.digits;
        return *this;
    }

    TBigInt operator+(const TBigInt &rhs) const {
        TBigInt copy = *this;
        copy += rhs;
        return copy;
    }

    TBigInt operator-(const TBigInt &rhs) const {
        TBigInt copy = *this;
        copy -= rhs;
        return copy;
    }

    TBigInt operator*(const TBigInt &rhs) const {
        TBigInt copy = *this;
        copy *= rhs;
        return copy;
    }

    TBigInt operator*(const int &rhs) const {
        TBigInt copy = *this;
        copy *= rhs;
        return copy;
    }

    TBigInt operator^(const TBigInt &rhs) const {
        TBigInt copy = *this;
        copy ^= rhs;
        return copy;
    }

    TBigInt operator/(const TBigInt &rhs) const {
        TBigInt copy = *this;
        copy /= rhs;
        return copy;
    }

    TBigInt operator/(const int &rhs) const {
        TBigInt copy = *this;
        copy /= rhs;
        return copy;
    }

    TBigInt &operator--() {
        *this -= TBigInt("1");
        return *this;
    }

    TBigInt operator--(int) {
        TBigInt copy = *this;
        --(*this);
        return copy;
    }

    void RemoveLeadingZeros() {
        while (this->digits.size() > 1 && this->digits.back() == 0) {
            this->digits.pop_back();
        }
    }
};

int main() {
    TBigInt BigNumber1;
    TBigInt BigNumber2;
    char operation;
    while (!std::cin.eof()) {
        std::cin >> BigNumber1 >> BigNumber2 >> operation;
        if (std::cin.eof())
            break;
        if (operation == '+') {
            std::cout << BigNumber1 + BigNumber2 << '\n';
        } else if (operation == '=') {
            if (BigNumber1 == BigNumber2)
                std::cout << "true" << '\n';
            else
                std::cout << "false" << '\n';
        } else if (operation == '>') {
            if (BigNumber1 > BigNumber2)
                std::cout << "true" << '\n';
            else
                std::cout << "false" << '\n';
        } else if (operation == '<') {
            if (BigNumber1 < BigNumber2)
                std::cout << "true" << '\n';
            else
                std::cout << "false" << '\n';
        } else if (operation == '-') {
            try {
                std::cout << BigNumber1 - BigNumber2 << '\n';
            } catch (TBigInt_exception i) {
                std::cout << "Error" << '\n';
            }
        } else if (operation == '*') {
            std::cout << BigNumber1 * BigNumber2 << '\n';
        } else if (operation == '/') {
            try {
                std::cout << BigNumber1 / BigNumber2 << '\n';
            } catch (TBigInt_exception i) {
                if (i == DIV_BY_ZERO)
                    std::cout << "Error" << '\n';
            }
        } else if (operation == '^') {
            try {
                std::cout << (BigNumber1 ^ BigNumber2) << '\n';
            } catch (TBigInt_exception i) {
                if (i == EXPONENTIATION_TO_ZERO)
                    std::cout << "Error" << '\n';
            }
        }
    }
}
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
        std::vector<int> res(n + m, 0);
        int carry;
        for (int i = 0; i < n; ++i) {
            carry = 0;
            for (int j = 0; j < m || carry; ++j) {
                long long cur = res[i + j] + carry + 1ll * this->digits[i] * (j < m ? rhs.digits[j] : 0);
                res[i + j] = cur % BASE;
                carry = cur / BASE;
            }
        }
        this->digits = res;
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
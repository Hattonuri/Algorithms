#include <cmath>
#include <iostream>

struct Complex {
  long double re = 0, im = 0;
  Complex(long double _re = 0, long double _im = 0) : re(_re), im(_im) {}

  double abs() const {
      return sqrt(re * re + im * im);
  }

  Complex operator+(const Complex other) const {
      return Complex(re + other.re, im + other.im);
  }
  Complex operator-(const Complex other) const {
      return Complex(re - other.re, im - other.im);
  }
  Complex operator*(const Complex other) const {
      return Complex(re * other.re - im * other.im, re * other.im + im * other.re);
  }
  Complex operator/(const Complex other) const {
      long double z = other.re * other.re + other.im * other.im;
      return Complex((re * other.re + im * other.im) / z, (im * other.re - re * other.im) / z);
  }
  bool operator==(const Complex other) const {
      return ((re == other.re) && (im == other.im));
  }
  bool operator!=(const Complex other) const {
      return !(*this == other);
  }
};

Complex operator-(const long double ch, const Complex other) {
    return Complex(ch) - other;
}

Complex operator+(const long double ch, const Complex other) {
    return Complex(ch) + other;
}

Complex operator*(const long double ch, const Complex other) {
    return Complex(ch) * other;
}

Complex operator/(const long double ch, const Complex other) {
    return Complex(ch) / other;
}

Complex operator==(const long double ch, const Complex other) {
    return Complex(ch) == other;
}

Complex operator!=(const long double ch, const Complex other) {
    return Complex(ch) != other;
}

std::ostream& operator<<(std::ostream &out, const Complex ch) {
    out << ch.re << ' ' << ch.im;
    return out;
}

std::istream& operator>>(std::istream &in, Complex &ch) {
    in >> ch.re >> ch.im;
    return in;
}

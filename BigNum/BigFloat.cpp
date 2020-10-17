#include"BigFloat.h"

namespace BigNum {
	BigFloat&BigFloat::operator=(const BigFloat&rhs) { numerator = rhs.numerator; denominator = rhs.denominator; convertToFraction(); return*this; }
	BigFloat&BigFloat::operator+=(const BigFloat&rhs) { numerator += rhs.numerator*denominator / rhs.denominator; convertToFraction(); return*this; }
	BigFloat&BigFloat::operator-=(const BigFloat&rhs){ numerator -= rhs.numerator*denominator / rhs.denominator; convertToFraction(); return*this; }
	BigFloat&BigFloat::operator*=(const BigFloat&rhs) { numerator *= rhs.numerator; denominator *= rhs.denominator; convertToFraction(); return*this; }
	BigFloat&BigFloat::operator/=(const BigFloat&rhs){ numerator *= rhs.denominator; denominator *= rhs.numerator; convertToFraction(); return*this; }
	BigFloat BigFloat::operator+(const BigFloat&rhs)const { auto a = *this; return a += rhs; }
	BigFloat BigFloat::operator-(const BigFloat&rhs)const{ auto a = *this; return a -= rhs; }
	BigFloat BigFloat::operator*(const BigFloat&rhs)const{ auto a = *this; return a *= rhs; }
	BigFloat BigFloat::operator/(const BigFloat&rhs)const{ auto a = *this; return a /= rhs; }
	BigFloat BigFloat::operator++(int) { auto a = *this; *this += 1; return a; }//increment postfix
	BigFloat BigFloat::operator--(int) { auto a = *this; *this -= 1; return a; }//decrement postfix
	BigFloat&BigFloat::operator++() { return*this += 1; }//increment prefix
	BigFloat&BigFloat::operator--() { return*this -= 1; }//decrement prefix

	BigFloat pow(const BigFloat&x, const BigFloat&y) { return BigFloat(root(pow(x.numerator, y.numerator), y.denominator), root(pow(x.denominator, y.numerator), y.denominator)).convertToFraction(); }
	BigFloat root(const BigFloat&x,const BigFloat&y) { return BigFloat(root(pow(x.numerator, y.denominator), y.numerator), root(pow(x.denominator, y.denominator), y.numerator)).convertToFraction(); }

	BigFloat::BigFloat(const BigInt&_numerator, const BigInt&_denominator) {
		numerator = _numerator; denominator = _denominator;
		convertToFraction();
	}

	BigFloat&BigFloat::convertToFraction(){
		auto gcd = GCD(numerator, denominator);
		numerator /= gcd;
		denominator /= gcd;
		return*this;
	}
	BigFloat::BigFloat(const std::string&rhs) {
		setZero();
		numerator.positiv = (rhs[0] != '-');
		bool afterdot = 0;
		for (unsigned long long i = !numerator.positiv; rhs[i]; i++) {
			if (rhs[i] > '9' || rhs[i] < '0')if (rhs[i] == '.') { afterdot = 1; continue; }else break;
			numerator *= 10;
			numerator += rhs[i] - '0';
			if (afterdot)denominator *= 10;
		}
		convertToFraction();
	}
	std::string BigFloat::getNumDec(unsigned long long max_decimals)const{
		std::string num; BigInt number, x = numerator, y = denominator; x.clear_error(); y.clear_error();
		unsigned long long i = 0;
		do {
			number = x*pow(BigInt(10), i) / y;
		} while (number.errors.size() && i++ < max_decimals);
		num = number.getNumDec();
		if (i - 1)num.insert(num.end() - i + 1, '.');
		return num;
	}
}
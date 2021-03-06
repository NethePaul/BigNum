#include"BigFloat.h"

namespace BigNum {
	BigFloat&BigFloat::operator=(const BigFloat&rhs) { numerator = rhs.numerator; denominator = rhs.denominator; convertToFraction(); return*this; }
	BigFloat&BigFloat::operator+=(const BigFloat&rhs) {
		numerator *= rhs.denominator;
		numerator += rhs.numerator*denominator;
		denominator *= rhs.denominator;
		return convertToFraction();
	}
	BigFloat&BigFloat::operator-=(const BigFloat&rhs){
		numerator *= rhs.denominator;
		numerator -= rhs.numerator*denominator;
		denominator *= rhs.denominator;
		return convertToFraction();
	}
	BigFloat&BigFloat::operator*=(const BigFloat&rhs) { numerator *= rhs.numerator; denominator *= rhs.denominator; convertToFraction(); return*this; }
	BigFloat&BigFloat::operator/=(const BigFloat&rhs){
		numerator *= rhs.denominator;
		denominator *= rhs.numerator;
		convertToFraction(); 
		return*this; }
	BigFloat BigFloat::operator+(const BigFloat&rhs)const { auto a = *this; return a += rhs; }
	BigFloat BigFloat::operator-(const BigFloat&rhs)const{ auto a = *this; return a -= rhs; }
	BigFloat BigFloat::operator*(const BigFloat&rhs)const{ auto a = *this; return a *= rhs; }
	BigFloat BigFloat::operator/(const BigFloat&rhs)const{ auto a = *this; return a /= rhs; }
	BigFloat BigFloat::operator++(int) { auto a = *this; *this += 1; return a; }//increment postfix
	BigFloat BigFloat::operator--(int) { auto a = *this; *this -= 1; return a; }//decrement postfix
	BigFloat&BigFloat::operator++() { return*this += 1; }//increment prefix
	BigFloat&BigFloat::operator--() { return*this -= 1; }//decrement prefix

	BigFloat pow(const BigFloat&x, const BigFloat&y, const BigInt&accuracy) {
		BigFloat n;
		if (y >= 0)
			if (y != 0)
				n = BigFloat(pow(x.numerator, y.numerator), pow(x.denominator, y.numerator));
			else {
				if (x != 0)
					n = 1;
				else n.adderror(Error::fatal::division_by_zero);
				return n;
			}
		else
			n = BigFloat(pow(x.denominator, abs(y.numerator)), pow(x.numerator, abs(y.numerator)));
		if (y.denominator != 1)
			return root(n, BigFloat(abs(y.denominator), 1), accuracy);
		return n;
	}
	BigFloat root(const BigFloat&x,const BigFloat&y,const BigInt&accuracy) {
		BigFloat A = y.denominator == 1 ? x : pow(x, BigFloat(y.denominator, 1));
		const BigFloat n = y.numerator;
		if (n == 0)if (x.numerator == 0) { BigFloat a; a.adderror(Error::fatal::invalid_root); return a; }else return 1;
		if (x == 0)return 0;
		if (x == 1)return 1;
		if (x < 0 && n.numerator % 2) { BigFloat n; n.adderror(Error::fatal::root_of_negativ_number); return n; };
		if (n == 1)return x;
		if (x == -1)return -1;
		BigFloat guess(root(x.numerator, y.numerator), root(x.denominator, y.numerator));
		bool is_perfect = 0;
		auto a = pow(BigFloat(1, 2), accuracy);
		auto is_approximately = [&]() {
			auto b = pow(guess, n) - A;
			if (b == 0) { is_perfect = 1; return true; }
			auto c = abs(pow(guess + a, n) - A);
			auto d = abs(pow(guess - a, n) - A);
			if (abs(b) < c && abs(b) < d)
				return true;
			if (b < 0)b -= a/2;
			else if (b > 0)b += a/2;
			return false;
		};
		while(!is_approximately()) {
			guess = ((y - 1)*guess + A / pow(guess, n-1)) / n;//the denominator of n is set to const 1 therefor pow will not call root and create an infinit loop
		}
		return guess;
	}
	BigFloat::BigFloat(const BigInt&_numerator, const BigInt&_denominator) {
		numerator = _numerator; denominator = _denominator;
		convertToFraction();
	}

	BigFloat&BigFloat::convertToFraction(){
		if (denominator == 0)adderror(Error::fatal::division_by_zero);
		auto gcd = GCD(numerator, denominator);
		numerator /= gcd;
		denominator /= gcd;
		return*this;
	}
	BigFloat::BigFloat(const std::string&rhs) {
		setZero();
		auto buffer = *this = 0;
		buffer.numerator.positiv = (rhs[0] != '-');
		bool afterdot = 0, is_fraction = 0;
		for (unsigned long long i = !buffer.numerator.positiv; rhs[i]; i++) {
			if (rhs[i] > '9' || rhs[i] < '0')
				if (rhs[i] == '.') { afterdot = 1; continue; }
				else if (rhs[i] == '/') { afterdot = 0; if (is_fraction)*this /= buffer; else *this = buffer; is_fraction = 1; buffer = 0; continue; }
				else break;
			buffer.numerator *= 10;
			buffer.numerator += rhs[i] - '0';
			if (afterdot)buffer.denominator *= 10;
		}
		if (is_fraction)*this /= buffer;
		else *this = buffer;
		convertToFraction();
	}
	BigFloat log(const BigFloat&x, const BigFloat&y,BigInt accuracy) {//approximating log
		auto log_=[](const BigFloat&x, const BigFloat&y) {//iterative log
			BigInt i = 0; auto buffer = y;
			while (buffer < x) {
				buffer *= y;
				i++;
				if (buffer == x)return i+1;
			}
			return i;
		};
		auto i_log = log_(x, y);
		auto remainder = x;
		auto result = BigFloat(i_log);
		auto digit = y;
		while (accuracy--) {
			remainder /= pow(y, i_log);
			remainder = pow(remainder, y);
			i_log = log_(remainder,y);
			result += BigFloat(i_log) / digit;
			digit *= y; 
		}
		return result;
	}

	std::string BigFloat::getNumDec(unsigned long long max_decimals)const{
		std::string num; BigInt number, x = numerator, y = denominator; x.clear_error(); y.clear_error();
		number = x*pow(BigInt(10), BigInt(max_decimals, false)+1) / y;
		num = number.getNumDec();
		if (num[0] == '0')return num;
		if (max_decimals<num.size())num.insert(num.end() - max_decimals-1, '.');
		while (num.back() == '0')num.pop_back();//erase unneccessary 0s after dot
		if (num.back() == '.')num.pop_back();//if nothing is written after the dot erase it to
		return num;
	}
	std::string BigFloat::getFractionDec()const {return numerator.getNumDec() + '/' + denominator.getNumDec();}
	std::string BigFloat::getFractionHex()const {return numerator.getNumHex() + '/' + denominator.getNumHex();}
	
	bool BigFloat::operator> (const BigFloat&rhs)const{ return numerator*rhs.denominator >  rhs.numerator*denominator; }
	bool BigFloat::operator>=(const BigFloat&rhs)const{ return numerator*rhs.denominator >= rhs.numerator*denominator; }
	bool BigFloat::operator< (const BigFloat&rhs)const{ return numerator*rhs.denominator <  rhs.numerator*denominator; }
	bool BigFloat::operator<=(const BigFloat&rhs)const{ return numerator*rhs.denominator <= rhs.numerator*denominator; }
	bool BigFloat::operator==(const BigFloat&rhs)const{ return numerator*rhs.denominator == rhs.numerator*denominator; }
	bool BigFloat::operator!=(const BigFloat&rhs)const{ return numerator*rhs.denominator != rhs.numerator*denominator; }

	BigFloat abs(const BigFloat&rhs) { return BigFloat(abs(rhs.numerator), abs(rhs.denominator)); }
}
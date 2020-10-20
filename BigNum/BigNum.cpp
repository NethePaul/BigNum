#pragma once
#include"BigNum.h"
#include<iostream>

namespace num1 {
	std::string IntToStr(unsigned int in)
	{
		if (!in)return"0";
		std::string buffer;
		while (in) {
			buffer += (in % 10) + '0';
			in /= 10;
		}
		return buffer;
	}
}


#define HIPART(halfsize,value)(((halfsize*)&value)[1])
#define LOPART(halfsize,value)(((halfsize*)&value)[0])


namespace stdVector {
	template<class T>
	void merge(std::vector<T>&lhs, const std::vector<T>&rhs) {
		if (&lhs == &rhs)return;
		lhs.reserve(rhs.size());
		for (int i = 0; i < rhs.size(); i++) {
			lhs.push_back(rhs[i]);
		}
	}
}



#define min(x,y)((x)<(y)?(x):(y))
#define max(x,y)((x)>(y)?(x):(y))

namespace BigNum {
	void BigInt::setZero() {
		value.clear();
		value.push_back(0);
		positiv = 1;
	}
	void BigInt::clear_back() {
		while (value.size() && !value.back())value.pop_back();//0x00000000F -> 0xF
		if (!value.size()) {
			positiv = 1;
			value.push_back(0);
		}
	}
	void BigInt::clear_error() {
		errors = 0;
	}
	std::string BigInt::getNumDec()const {
		unsigned long long d, r;
		std::vector<type>buffer = value;
		auto all_zero = [&buffer]() {
			for (unsigned long long i = 0; i < buffer.size(); i++)
				if (buffer[i])
					return false;
			return true;
		};
		std::string num;
		do {
			r = buffer.back();
			if (buffer.size() >= 2) {
				for (ltype i = buffer.size() - 1; i--;) {
					d = r / 10;
					r = ((r - d * 10) << sizeof(type) * 8) + buffer[i];
					buffer[i + 1] = d;
				}
			}
			d = r / 10;
			r = r - d * 10;
			buffer[0] = d;

			num = num1::IntToStr((unsigned int)r) + num;

		} while (!all_zero());
		if (!positiv)num.insert(num.begin(), '-');
		return num;
	}
	std::string BigInt::getNumHex()const {
		std::string buffer;
		for (unsigned long long j = value.size(); j--;)
			for (unsigned long long i = sizeof(type) * 2; i--;) {
				type buffer2 = value[j];
				buffer2 = buffer2&(0xF << i * 4);
				buffer.push_back(Hex_from_4Bit(buffer2 >> i * 4));
			}
		return buffer;
	}
	char BigInt::Hex_from_4Bit(unsigned short in)const {
		in %= 16;
		if (in < 10)return '0' + in;
		return 'A' + in - 10;
	}

	BigInt::BigInt(signed long long rhs) :positiv(rhs > 0),errors(0) {
		if (!positiv)rhs *= -1;

		if (!rhs)goto jmp;
		value.push_back(((type*)&rhs)[0]);
		value.push_back(((type*)&rhs)[1]);
		if (!value.back())value.pop_back();//0x00000000F -> 0xF
		return;
	jmp:
		positiv = 1;
		value.push_back(0);
	}
	BigInt::BigInt(const std::string&rhs): errors(0) {
		setZero();
		positiv = (rhs[0] != '-');
		for (ltype i = !positiv; rhs[i]; i++) {
			if (rhs[i] > '9' || rhs[i] < '0')break;
			*this *= 10;
			*this += rhs[i] - '0';

		}
	}
	BigInt::BigInt(ltype rhs, bool) :positiv(1), errors(0) {
		if (!rhs)goto jmp;
		value.push_back(((type*)&rhs)[0]);
		value.push_back(((type*)&rhs)[1]);
		clear_back();
		return;
	jmp:
		positiv = 1;
		value.push_back(0);
	}
	BigInt::BigInt(const BigInt&rhs) :positiv(rhs.positiv), errors(rhs.errors), value(rhs.value) {
		clear_back();
	}

	BigInt&BigInt::add(const BigInt&rhs, ltype P) {
		bool a = 0;
		if (P > value.size()) {
			value.reserve(P - value.size());
			a = 1;
		}
		for (unsigned long long i = 0; i < rhs.value.size(); i++) {
			ltype buffer = rhs.value[i];
			for (unsigned long long j = i + P; buffer; j++) {
				if (value.size() > j)goto add;
				while (j > value.size())value.push_back(0);
				value.push_back(buffer);
				break;
			add:
				buffer += value[j];
				value[j] = LOPART(type, buffer);
				buffer = HIPART(type, buffer);
			}
		}
		if (a)
			value.shrink_to_fit();
		return*this;
	}
	BigInt&BigInt::sub(const BigInt&rhs) {
		ltype buffer = 0;
		bool b;
		for (unsigned long long i = 0; (b = i < rhs.value.size()) || buffer; i++) {
			if (b)
				buffer += rhs.value[i];
			if (buffer) {
				if (value.size() > i)goto sub;
				adderror(Error::fatal::invalid_function_call);
				return*this;
			sub:

				//buffer += _UI32_MAX  -  value[j];
				if (HIPART(type, buffer))//if an integer overflow occured last interval add
					buffer++;
				bool a = value[i] < LOPART(type, buffer);//if an integer overflow is going to occure

				value[i] -= LOPART(type, buffer);

				LOPART(type, buffer) = 0;
				HIPART(type, buffer) = a;
			}
		}
		clear_back();
		return*this;
	}

	BigInt&BigInt::mul(const BigInt&rhs) {
		BigInt buffer = 0;
		ltype buffer2 = 0;
		buffer.value.clear();
		buffer.value.reserve(value.size() + rhs.value.size());
		for (unsigned long long i = 0; i < value.size() + rhs.value.size(); i++)
			buffer.value.push_back(0);

		for (unsigned long long i = rhs.value.size(); i--;) {
			for (unsigned long long j = value.size(); j--;) {
				buffer2 = (unsigned long long)value[j] * (unsigned long long)rhs.value[i];
				buffer.add(BigInt(buffer2, false), j + i);
			}
		}
		buffer.clear_back();
		return*this = buffer;
	}
	BigInt&BigInt::div(const BigInt&rhs, BigInt*rest) {
		BigInt buffer = *this;
		BigInt buffer2 = rhs;
		BigInt b = buffer2;
		buffer.positiv = buffer2.positiv = 1;
		setZero();

		if (rhs == 0) { adderror(Error::fatal::division_by_zero); return*this; }


		ltype i, j;
		while (buffer >= buffer2) {

			b = 0;
			size_t l = buffer2.value.size();
			auto a = (buffer.value.back() < buffer2.value.back());
			i = buffer.value.size() - l - a;
			b.add(buffer2, i);
			l--;
			if (!a)
				j = buffer.value[i + l] / b.value[i + l];
			else
				j = (((unsigned long long)buffer.value.back())<<(sizeof(ltype)*4)) / b.value[i + l];
			if (b*j > buffer) {
				if (!l||buffer2.value[l - 1]) {
					if (!a)
						j = buffer.value[i + l] / (b.value[i + l] + 1);
					else
						j = (((unsigned long long)buffer.value.back()) << (sizeof(ltype) * 4)) / (b.value[i + l]+1);
				}
				else j--;
			}
			if (j)
				b *= j;
			else
				j = 1;
			buffer.sub(b);
			this[0].add(j, i);


			if (buffer < buffer2)break;
		}
		if (rest)
			*rest = buffer;
		else if (buffer != 0)
			adderror(Error::nonfatal::integer_division_accuracy_loss);

		return*this;
	}
	BigInt&BigInt::mod(const BigInt&rhs) {
		BigInt buffer = *this;
		buffer.div(rhs, this);
		return *this;
	}

#undef c


	bool BigInt::operator>(const BigInt&rhs)const {
		BigInt bthis = *this;
		BigInt brhs = rhs;
		bthis.clear_back();
		brhs.clear_back();
		if (bthis.positiv != brhs.positiv)return bthis.positiv > brhs.positiv;
		if (bthis.value.size() != brhs.value.size())return bthis.value.size() > brhs.value.size();

		for (unsigned long long i = bthis.value.size(); i--;)
			if (bthis.value[i] != brhs.value[i])
				return bthis.value[i] > brhs.value[i];

		return false;
	}
	bool BigInt::operator>=(const BigInt&rhs)const {
		return this[0] > rhs || this[0] == rhs;
	}
	bool BigInt::operator<(const BigInt&rhs)const {
		return!(this[0] >= rhs);
	}
	bool BigInt::operator<=(const BigInt&rhs)const {
		return!(this[0] > rhs);
	}


	bool BigInt::operator==(const BigInt&rhs)const {
		BigInt bthis = *this;
		BigInt brhs = rhs;
		bthis.clear_back();
		brhs.clear_back();
		if (bthis.positiv != brhs.positiv)return false;
		if (bthis.value.size() != brhs.value.size())return false;

		for (unsigned long long i = bthis.value.size(); i--;)
			if (bthis.value[i] != brhs.value[i])
				return false;

		return true;
	}
	bool BigInt::operator!=(const BigInt&rhs)const {
		return!(this[0] == rhs);
	}

	BigInt hyper(unsigned long long l, const BigInt&a, const BigInt&b)//hyper operator
	{
		if (l == 0)return b + 1;
		if (l == 1)return a + b;
		if (l == 2)return a*b;
		if (!a.positiv) {
			auto d = a; d.positiv = 1;
			auto e = b; e.positiv = !b.positiv;

			BigInt c(d); e--;
			for (BigInt i = 0; i < e; i++)
				c = hyper(l - 1, c, d);
			return c;
		}
		BigInt c(a);
		for (BigInt i = 0; i < b - 1; i++)
			c = hyper(l - 1, c, a);
		return c;
	}
	BigInt pow(const BigInt&x, const BigInt&y) {
		auto buffer = x; auto X = x; X.clear_error();
		auto buffery = y;
		if (y == 0)if (x == 0) { buffer = 1; buffer.adderror(Error::fatal::division_by_zero); return buffer; }else return 1;
		if (x == -1)return y%-2;
		if (x == 0 || x == 1)return x;
		if (y < 0) { buffer = 0; buffer.adderror(Error::nonfatal::integer_division_accuracy_loss); return buffer; }
		
		
		while (buffery--) {
			BigInt y2 = 2;
			for (BigInt i = X; (buffery -= y2) > 0; ) {
				i *= i;
				buffer *= i;
				y2 <<= 1;
			}
			buffery += y2;
			buffer *= X;
		}
		
		return buffer;
	}

	BigInt&BigInt::operator=(const BigInt&rhs) {
		positiv = rhs.positiv; errors = rhs.errors; value = rhs.value;
		while (!value.size() && !value.back())value.pop_back();//0x00000000F -> 0xF
		if (!value.size()) {
			positiv = 1;
			value.push_back(0);
		}
		return*this;
	}
	BigInt&BigInt::operator+=(const BigInt&rhs) {
		adderror( rhs.errors);
		if (positiv == rhs.positiv)return add(rhs);
		if (this[0] > rhs)
			return sub(rhs);
		if (this[0] < rhs);
		{
			positiv = !positiv;
			auto buffer(rhs);
			return this[0] = buffer.sub(*this);
		}
		//if this=rhs
		value.clear();
		value.push_back(0);
		positiv = 1;
		return*this;
	}
	BigInt&BigInt::operator-=(const BigInt&rhs) {
		adderror( rhs.errors);
		if (positiv != rhs.positiv)return add(rhs);
		if (*this > rhs)
			return sub(rhs);
		if (*this < rhs)
		{
			positiv = !positiv;
			auto buffer(rhs);
			value = buffer.sub(*this).value;
			return*this;
		}
		//if this=rhs
		value.clear();
		value.push_back(0);
		positiv = 1;
		return*this;
	}
	BigInt&BigInt::operator*=(const BigInt&rhs) {
		adderror( rhs.errors);
		auto a = positiv;
		mul(rhs);
		positiv = !(a^rhs.positiv);
		return*this;
	}
	BigInt&BigInt::operator/=(const BigInt&rhs) {
		adderror( rhs.errors);
		bool buffer = positiv;
		div(rhs);
		if (this[0] == 0)
		{
			setZero();
			return*this;
		}
		positiv = !(buffer^rhs.positiv);

		return*this;
	}
	BigInt&BigInt::operator%=(const BigInt&rhs) {
		adderror( rhs.errors);
		bool buffer = positiv;

		mod(rhs);
		if (this[0] == 0)
		{
			setZero();
			return*this;
		}
		positiv = !(buffer^rhs.positiv);
		return*this;
	}
	BigInt BigInt::operator%(const BigInt&rhs)const {
		auto buffer = *this;
		return buffer%=rhs;
	}

	BigInt BigInt::operator+(const BigInt&rhs)const {
		auto buffer = this[0];
		return buffer += rhs;
	}
	BigInt BigInt::operator-(const BigInt&rhs)const {
		auto buffer = this[0];
		return buffer -= rhs;
	}
	BigInt BigInt::operator*(const BigInt&rhs)const {
		auto buffer = this[0];
		return buffer *= rhs;
	}
	BigInt BigInt::operator/(const BigInt&rhs)const {
		auto buffer = this[0];
		return buffer /= rhs;
	}

	BigInt BigInt::operator++(int) {
		auto r = *this += 1;
		return r;
	}
	BigInt BigInt::operator--(int) {
		auto r = *this -= 1;
		return r;
	}
	BigInt&BigInt::operator++() {return*this += 1;}
	BigInt&BigInt::operator--() {return*this -= 1;}

	BigInt&BigInt::operator<<=(ltype rhs) {
		short b = rhs % (sizeof(type) * 8);
		rhs /= sizeof(type) * 8;
		clear_back();
		if (rhs + value.size() < rhs) {//overflow
			adderror(Error::fatal::bignum_overflow);
			return*this = 0;
		}
		std::vector<type>a; a.reserve(rhs);
		for (unsigned long long i = rhs; i--;)
			a.push_back(0);
		stdVector::merge(a, value); value = a;

		type buffer = 0;
		for (unsigned long long j = 0; j < value.size(); j++) {
			type buffer2 = value[j];
			value[j] <<= b;
			value[j] += (buffer >> sizeof(type) * 8 - b);
			buffer = 0;

			for (unsigned short i = b; i--;)
				buffer += (buffer2 & (0x1 << (sizeof(type) * 8 - i - 1)));

		}if (buffer)value.push_back(buffer >> sizeof(type) * 8 - b);
		clear_back();

		return*this;
	}
	BigInt&BigInt::operator>>=(ltype rhs) {
		unsigned short b = rhs % (sizeof(type) * 8);
		rhs /= sizeof(type) * 8;
		clear_back();
		if (value.size() - rhs > value.size()) {
			return*this = 0;
		}
		for (unsigned long long i = 0; i < value.size() - rhs; i++)
			value[i] = value[i + rhs];
		clear_back();
		type buffer = 0;
		for (unsigned long long j = value.size(); j--;) {
			type buffer2 = value[j];
			value[j] >>= b;
			value[j] += (buffer << sizeof(type) * 8 - b);
			buffer = 0;
			for (unsigned short i = b; i--;)
				buffer += buffer2 & (0x1 << (i));

		}
		clear_back();
		return*this;
	}
	BigInt BigInt::operator >> (ltype in)const {
		auto b = *this;
		return b >>= in;
	}
	BigInt BigInt::operator<<(ltype in)const {
		auto b = *this;
		return b <<= in;
	}

	BigInt&BigInt::operator&=(const BigInt&rhs) {//bitwise and
		while (rhs.value.size() < value.size())value.pop_back();
		for (unsigned long long i = value.size(); i--;)value[i] &= rhs.value[i];
		return*this;
	}
	BigInt&BigInt::operator|=(const BigInt&rhs) {//bitwise or
		for (unsigned long long i = min(value.size(), rhs.value.size()); i--;)value[i] |= rhs.value[i];
		return*this;
	}
	BigInt&BigInt::operator^=(const BigInt&rhs) {//bitwise xor
		for (unsigned long long i = min(value.size(), rhs.value.size()); i--;)value[i] ^= rhs.value[i];
		return*this;
	}
	BigInt BigInt::operator~()const {//bitwise not
		auto b = *this; b.clear_back();
		for (unsigned long long i = value.size(); i--;)b.value[i] = ~(value[i]);
		b.clear_back();
		return b;
	}
	BigInt BigInt::operator&(const BigInt&rhs)const {//bitwise and
		auto a = *this;
		return a &= rhs;
	}
	BigInt BigInt::operator|(const BigInt&rhs)const {//bitwise or
		auto a = *this;
		return a |= rhs;
	}
	BigInt BigInt::operator^(const BigInt&rhs)const {//bitwise xor
		auto a = *this;
		return a ^= rhs;
	}

	BigInt root(const BigInt&x, const BigInt&y) {//yth root of
		if (y == 0)
			if (x == 0) { BigInt r = 1; r.adderror(Error::fatal::invalid_root); return r; }
			else return 1;
		if (x == 0 || x == 1) return x;
		BigInt r = x;
		if (x < 0&&y%2) { r.adderror(Error::fatal::root_of_negativ_number); return r; }
		const BigInt y2 = y - 1;
		BigInt guess = 2;
		auto is_approximately = [&](bool&perfect) {
			auto b = x - pow(guess, y);
			if (b == 0) { perfect = 1; return true; }
			else perfect = 0;
			auto c = abs(pow(guess + 1, y) - x);
			auto d = abs(pow(guess - 1, y) - x);
			if (abs(b) <= c && abs(b) <= d) 
				return true;
			return false;
		};
		bool is_perfect;
		do {
			guess.clear_error();
			auto pguess = guess;
			guess = (y2 * guess + x / pow(guess, y2)) / y;
			if (guess == pguess) { guess.errors = x.errors; guess.adderror(Error::nonfatal::integer_root_accuracy_loss); return guess; }
		} while (!is_approximately(is_perfect));
		guess.errors = x.errors;
		if (!is_perfect)guess.adderror(Error::nonfatal::integer_root_accuracy_loss);
		guess.adderror(y.errors);
		return guess;
	}
	BigInt abs(const BigInt&x) {auto a = x; a.positiv = 1; return a;}
	BigInt log(const BigInt&x, const BigInt&base) {//no optimizations yet
		BigInt B = base;
		BigInt i = 1;
		if (abs(base) == 1 || base == 0) { i.adderror(Error::fatal::log_to_invalid_base); i.adderror(x.errors); i.adderror( base.errors); return i; }
		while (B < x) { B *= base; i++; };
		if (B != x) { i.adderror(Error::nonfatal::integer_log_accuracy_loss); i--; }
		i.adderror(x.errors);
		i.adderror(base.errors);
		return i;
	}
	BigInt GCD(BigInt a, BigInt b)
	{
		BigInt h;
		if (a == 0) return abs(b);
		if (b == 0) return abs(a);
		do {
			h = a % b;
			a = b;
			b = h;
		} while (b != 0);
		return abs(a);
	}
	BigInt LCM(const BigInt&x, const BigInt&y) {return abs(x*y) / GCD(x, y);}
}
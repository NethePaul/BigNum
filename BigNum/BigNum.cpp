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
		errors.clear();
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
			for (unsigned long long i = buffer.size() - 2; i >= 0; i--) {
				d = r / 10;
				r = ((r - d * 10) << sizeof(type) * 8) + buffer[i];
				buffer[i + 1] = d;
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

	BigInt::BigInt(signed long long rhs) :positiv(rhs > 0) {
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

	BigInt::BigInt(ltype rhs, bool) :positiv(1) {
		if (!rhs)goto jmp;
		value.push_back(((type*)&rhs)[0]);
		value.push_back(((type*)&rhs)[1]);
		while (!value.back())value.pop_back();//0x00000000F -> 0xF
		return;
	jmp:
		positiv = 1;
		value.push_back(0);
	}
	BigInt::BigInt(const BigInt&rhs) :positiv(rhs.positiv), errors(rhs.errors), value(rhs.value) {
		clear_back();
	}

	BigInt&BigInt::add(const BigInt&rhs,ltype P) {
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
				errors.push_back(Error::fatal::invalid_function_call);
				return*this;
			sub:
				
				//buffer += _UI32_MAX  -  value[j];
				if (HIPART(type, buffer))//if an integer overflow occured last interval add
					buffer++;
				bool a = value[i] < LOPART(type,buffer);//if an integer overflow is going to occure

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
			for (unsigned long long j = value.size();j--;) {
				buffer2 = (unsigned long long)value[j] * (unsigned long long)rhs.value[i];
				buffer.add(BigInt(buffer2,false), j + i);
			}
		}
		buffer.clear_back();
		return*this = buffer;
	}
	BigInt&BigInt::div(const BigInt&rhs, BigInt*rest) {
		BigInt buffer = *this;
		BigInt buffer2 = rhs;
		BigInt b = buffer2;
		buffer.positiv=buffer2.positiv = 1;
		setZero();

		if (rhs == 0) { errors.push_back(Error::fatal::division_by_zero); return*this; }

		
		type i,j;
		while (buffer>buffer2) {

			b = 0;
			size_t l = buffer2.value.size();
			i = buffer.value.size() - l - (buffer.value.back() < buffer2.value.back());
			b.add(buffer2, i);
			l--;
			j = /*/1;/*/buffer.value[i+l] / b.value[i+l];

			if (b*j > buffer) {
				if (buffer2.value[l - 1]) {
					j = buffer.value[i + l] / (b.value[i + l] + 1);
				}
				else j--;
			}
			if (j)
				b *= j;
			else
				j = 1;
			//*/
			if (b > buffer) {
				i--;
				b = 0;
				b.add(buffer2, i);
				while (b*j <= buffer&&j<0x80000000)
					j *= 2;
				b *= j /= 2;
			}
			buffer.sub(b);
			this[0].add(j, i);


			if (buffer < buffer2)break;
		}
		if (rest)
			*rest = buffer;
		else if (buffer != 0)
			errors.push_back(Error::nonfatal::integer_division_accuracy_loss);

		return*this;
	}
	BigInt&BigInt::mod(const BigInt&rhs){
		BigInt buffer = *this;
		buffer.div(rhs,this);
		return *this;
	}

#undef c

	BigInt&BigInt::incr() {
		for (unsigned long long i = 0;; i++) {
			if (value.size() <= i) {
				value.push_back(1);
				return*this;
			}
			if (value[i] != _UI32_MAX) {
				value[i]++; return*this;
			}
		}
	}
	BigInt&BigInt::decr() {
		if (value.size() == 0) {
			positiv = 0;
			value.push_back(1);
			return*this;
		}
		for (unsigned long long i = 0; i < value.size(); i++)
			if (value[i])
				value[i]--;
		while (value.size()&&!value.back())value.pop_back();//0x00000000F -> 0xF
		if (!value.size()) {
			positiv = 1;
			value.push_back(0);
		}
		return*this;
	}


	bool BigInt::operator>(const BigInt&rhs)const {
		BigInt bthis = *this;
		BigInt brhs = rhs;
		bthis.clear_back();
		brhs.clear_back();
		if (bthis.positiv != brhs.positiv)return bthis.positiv > brhs.positiv;
		if (bthis.value.size() != brhs.value.size())return bthis.value.size()>brhs.value.size();

		for (long i = bthis.value.size()-1; i >= 0; i--)
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

		for (long i = bthis.value.size()-1; i >= 0; i--)
			if (bthis.value[i] != brhs.value[i])
				return false;

		return true;
	}
	bool BigInt::operator!=(const BigInt&rhs)const {
		return!(this[0] == rhs);
	}

	BigInt hyper(unsigned long long l,const BigInt&a,const BigInt&b)//hyper operator
	{
		//auto bufferr(rhs);
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
		for (BigInt i=0; i < b - 1; i++)
			c = hyper(l - 1, c, a);
		return c;
	}
	BigInt pow(const BigInt&x, const BigInt&y) {
		auto buffer = x;
		auto buffery = y;
		if (y == 0)return 1;
		if (x == 0)return 0;
		while (true) {
			BigInt buffer2 = 2;
			BigInt buffer3 = 2;
			if (buffer2 < buffery) {
				do {
					buffer2 = buffer3;
					buffer *= buffer;
					buffer3 = buffer2*buffer2;
				} while (buffer3 < buffery);
				buffery -= buffer2;
			}
			if (buffer2 == 2)break;
		}
		for (BigInt i = buffery; (i--)!=0;){// I would have overloaded the ".operator bool" but it caused syntax errors
			buffer *= x;
			buffery--;
		}
		return buffer;
	}

	BigInt&BigInt::operator=(const BigInt&rhs) { 
			positiv = rhs.positiv; errors = rhs.errors; value = rhs.value;
			while (!value.size()&&!value.back())value.pop_back();//0x00000000F -> 0xF
			if (!value.size()) {
				positiv = 1;
				value.push_back(0);
			}
			return*this;
		}
		BigInt&BigInt::operator+=(const BigInt&rhs) {
			stdVector::merge(errors, rhs.errors);
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
			stdVector::merge(errors, rhs.errors);
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
			stdVector::merge(errors, rhs.errors);
			positiv = !(positiv^rhs.positiv);
			return mul(rhs);
		}
		BigInt&BigInt::operator/=(const BigInt&rhs) {
			stdVector::merge(errors, rhs.errors);
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
		BigInt&BigInt::operator%=(const BigInt&rhs){
			stdVector::merge(errors, rhs.errors);
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
			return buffer%rhs;
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
			auto r = *this;
			if (positiv)incr();
			else decr();
			return r;
		}
		BigInt BigInt::operator--(int) {
			auto r = *this;
			if (positiv)decr();
			else incr();
			return r;
		}

		BigInt&BigInt::operator<<=(ltype rhs) {
			short b = rhs % (sizeof(type)*8);
			rhs /= sizeof(type)*8;
			clear_back();
			if (rhs + value.size() < rhs) {//overflow
				errors.push_back(Error::fatal::bignum_overflow);
				return*this = 0;
			}
			std::vector<type>a; a.reserve(rhs);
			for (unsigned long long i = rhs; i--;)
				a.push_back(0);
			stdVector::merge(a, value); value = a;

			type buffer = 0;
			for (unsigned long long j = 0; j<value.size(); j++) {
				type buffer2 = value[j];
				value[j] <<= b;
				value[j] += (buffer >> sizeof(type) * 8 - b);
				buffer = 0;

				for (unsigned short i = b; i--;)
					buffer+= (buffer2 & (0x1 << (sizeof(type) * 8 - i-1)));

			}if (buffer)value.push_back(buffer>>sizeof(type)*8-b);
			clear_back();

			return*this;
		}
		BigInt&BigInt::operator>>=(ltype rhs) {
			unsigned short b = rhs % (sizeof(type)*8);
			rhs /= sizeof(type)*8;
			clear_back();
			if (value.size()-rhs > value.size()) {
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
		BigInt BigInt::operator>>(ltype in)const {
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
			for (unsigned long long i = min(value.size(),rhs.value.size()); i--;)value[i] |= rhs.value[i];
			return*this;
		}
		BigInt&BigInt::operator^=(const BigInt&rhs) {//bitwise xor
			for (unsigned long long i = min(value.size(), rhs.value.size()); i--;)value[i] ^= rhs.value[i];
			return*this;
		}
		BigInt BigInt::operator!()const {//bitwise not
			auto b = *this;
			for (unsigned long long i = value.size(); i--;)b.value[i] = !value[i];
			return b;
		}
		BigInt BigInt::operator&(const BigInt&rhs)const {//bitwise and
			auto a = *this;
			return a &= rhs;
		}
		BigInt BigInt::operator|(const BigInt&rhs)const{//bitwise or
			auto a = *this;
			return a |= rhs;
		}
		BigInt BigInt::operator^(const BigInt&rhs)const{//bitwise xor
			auto a = *this;
			return a ^= rhs;
		}
}
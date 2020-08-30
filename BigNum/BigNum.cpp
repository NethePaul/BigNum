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
	



	BigInt&BigInt::add(const BigInt&rhs,unsigned int P) {
		bool a = 0;
		if (P > value.size()) {
			value.reserve(P - value.size());
			a = 1;
		}
		for (unsigned int i = 0; i < rhs.value.size(); i++) {
			ltype buffer = rhs.value[i];
			for (unsigned int j = i + P; buffer; j++) {
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
		for (unsigned int i = 0; i < rhs.value.size(); i++) {
			buffer += rhs.value[i];
			for (unsigned int j = i; buffer; j++) {
				if (value.size() > j)goto sub;
				errors.push_back(Error::fatal::invalid_function_call);
				return*this;
			sub:
				//buffer += _UI32_MAX  -  value[j];
				if (HIPART(type, buffer))
					buffer++;
				if (value[j] >= LOPART(type,buffer)) {
					value[j] -= LOPART(type, buffer);
					buffer = 0;
				}
				//8590000000
				else {
					value[j] -= LOPART(type, buffer);
					
					LOPART(type, buffer) = 0;
					HIPART(type, buffer) = 1;
				}
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
		for (unsigned int i = 0; i < value.size() + rhs.value.size(); i++)
			buffer.value.push_back(0);

		for (unsigned int i = 0; i < rhs.value.size(); i++) {
			for (unsigned int j = 0; j < value.size(); j++) {
				buffer2 = (ltype)value[j] * (ltype)rhs.value[i];
				buffer.add(buffer2, j + i);
			}
		}
		buffer.clear_back();
		return*this = buffer;
	}
	BigInt&BigInt::div(const BigInt&rhs) {
		BigInt buffer = *this;
		BigInt buffer2 = rhs;
		BigInt b = buffer2;
		buffer.positiv=buffer2.positiv = 1;
		setZero();

		if (rhs > buffer) { goto endloop; }
		if (rhs == 0) { errors.push_back(Error::fatal::division_by_zero); return*this; }

		
		type i,j;
	loop:
		j = 0;
		b = 0;
		i = buffer.value.size() - 1 - (buffer.value.back() < buffer2.value.back());
		b.add(buffer2, i);
		while (b*++j < buffer);
		//j = max(buffer.value[i] / b.value[i], 1);

		b *= --j;
		
		
		buffer.sub(b);
		
		this[0].add(j, i);
		


		if (buffer < buffer2)goto endloop;
		if (i > 0 || j > 1)goto loop;
	endloop:
		if (buffer != 0)
			errors.push_back(Error::nonfatal::integer_division_accuracy_loss);

		return*this;
	}

#undef c

	BigInt&BigInt::incr() {
		for (unsigned int i = 0;; i++) {
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
		for (unsigned int i = 0; i < value.size(); i++)
			if (value[i])
				value[i]--;
		while (!value.back())value.pop_back();//0x00000000F -> 0xF
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

	BigInt hyper(unsigned int l, BigInt a, BigInt b)//ackerman function //hyper operator
	{
		//auto bufferr(rhs);
		if (l == 0)return b + 1;
		if (l == 1)return a + b;
		if (l == 2)return a*b;
		if (!a.positiv) {
			b.positiv =! b.positiv;
			a.positiv = 1;
		}
		BigInt c(a);
		for (BigInt i=0; i < b - 1; i++)
			c = hyper(l - 1, c, a);
		return c;
	}


	
}
#pragma once
#include<vector>
#include<math.h>
#include<string>

#define HIPART(halfsize,value)(((halfsize*)&value)[1])
#define LOPART(halfsize,value)(((halfsize*)&value)[0])

//*
namespace num1 {
	std::string IntToStr(unsigned int in);
}//*/

namespace stdVector {
	template<class T>
	void merge(std::vector<T>&lhs,const std::vector<T>&rhs) {
		lhs.reserve(rhs.size());
		for (int i = 0; i < rhs.size();i++) {
			lhs.push_back(rhs[i]);
		}
	}
}

namespace BigNum {
	namespace Error {
		namespace fatal {
			enum {
				invalid_function_call,
				division_by_zero,
				log_to_invalid_base,
				invalid_root,
				root_of_negativ_number,
				//insert more fatal errors here
				non_fatal_first
			};
		}
		namespace nonfatal {
			enum {
				decimal_to_integer_accuracy_loss = fatal::non_fatal_first,
				integer_division_accuracy_loss,
				integer_root_accuracy_loss
				//insert more non fatal errors here
			};
		}
	}

	class BigFloat;
	class BigInt {
		using type = unsigned int;
		using ltype = unsigned long long;//long version of type

		std::vector<type>value;
		std::vector<short>errors;
		volatile bool positiv;

		
	private:
		//ignore sign{
		
		BigInt&add(const BigInt&,unsigned int P = 0);//addition
		BigInt&sub(const BigInt&);//subtraction
		BigInt&mul(const BigInt&);//multiplication
		BigInt&div(const BigInt&);//division
		BigInt&incr();//increment;
		BigInt&decr();//decrement

		//}
	public:
		BigInt&mod(const BigInt&);

	public:
		friend BigInt hyper(unsigned int l, BigInt a, BigInt b);//hyper operator //ackerman function
		

	public:
		bool operator>(const BigInt&)const;
		bool operator>=(const BigInt&)const;
		bool operator<(const BigInt&)const;
		bool operator<=(const BigInt&)const;



		bool operator==(const BigInt&)const;
		bool operator!=(const BigInt&)const;


	public:
		friend BigInt power(const BigInt&x,const BigInt&y);//power x^y
		friend BigInt root(const BigInt&x, const BigInt&y);//root yth root of x
		friend BigInt log(const BigInt&x, const BigInt&y);//logarithm log of x to the base of y

		friend class BigFloat;
	public:
		void setZero() {
			
			value.clear();
			value.push_back(0);
			positiv = 1;
		}
		void clear_back() {
			while (value.size() && !value.back())value.pop_back();//0x00000000F -> 0xF
			if (!value.size()) {
				positiv = 1;
				value.push_back(0);
			}
		}
		void clear_error() {
			errors.clear();
		}
		const auto&getErrors()const { return errors; }
		std::string getNumDec()const {
			unsigned long long d, r;
			std::vector<type>buffer = value;
			auto all_zero = [&buffer]() {
				for (unsigned int i = 0; i < buffer.size(); i++)
					if (buffer[i])
						return false;
				return true;
			};
			std::string num;
			do {
				r = buffer.back();
				for (int i = buffer.size()-2; i >=0; i--) {
					d = r / 10;
					r = ((r - d * 10) << sizeof(type)*8) + buffer[i];
					buffer[i+1] = d;
				}
				d = r / 10;
				r = r - d * 10;
				buffer[0] = d;

				num = num1::IntToStr((unsigned int)r) + num;

			} while (!all_zero());
			if (!positiv)num.insert(num.begin(), '-');
			return num;
		}
	public:
		//*
		BigInt(signed long long rhs) :positiv(rhs > 0) {
			if (!positiv)rhs*=-1;

			if (!rhs)goto jmp;
			value.push_back(((type*)&rhs)[0]);
			value.push_back(((type*)&rhs)[1]);
			if (!value.back())value.pop_back();//0x00000000F -> 0xF
			return;
		jmp:
			positiv = 1;
			value.push_back(0);
		}
		
		BigInt(ltype rhs,bool) :positiv(1) {
			if (!rhs)goto jmp;
			value.push_back(((type*)&rhs)[0]);
			value.push_back(((type*)&rhs)[1]);
			while (!value.back())value.pop_back();//0x00000000F -> 0xF
			return;
			jmp:
			positiv = 1;
			value.push_back(0);
		}//*/
		BigInt(const BigInt&rhs):positiv(rhs.positiv),errors(rhs.errors),value(rhs.value) {
			clear_back();
		}
	public:
		BigInt&operator=(const BigInt&rhs) { 
			positiv = rhs.positiv; errors = rhs.errors; value = rhs.value;
			while (!value.size()&&!value.back())value.pop_back();//0x00000000F -> 0xF
			if (!value.size()) {
				positiv = 1;
				value.push_back(0);
			}
			return*this;
		}
		BigInt&operator+=(const BigInt&rhs) {
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
		BigInt&operator-=(const BigInt&rhs) {
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
		BigInt&operator*=(const BigInt&rhs) {
			stdVector::merge(errors, rhs.errors);
			positiv = positiv^rhs.positiv;
			return mul(rhs);
		}
		BigInt&operator/=(const BigInt&rhs) {
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


		BigInt operator+(const BigInt&rhs)const {
			auto buffer = this[0];
			return buffer += rhs;
		}
		BigInt operator-(const BigInt&rhs)const {
			auto buffer = this[0];
			return buffer -= rhs;
		}
		BigInt operator*(const BigInt&rhs)const {
			auto buffer = this[0];
			return buffer *= rhs;
		}
		BigInt operator/(const BigInt&rhs)const {
			auto buffer = this[0];
			return buffer /= rhs;
		}

		BigInt&operator++(int) {
			if (positiv)return incr();
			return decr();
		}
		BigInt&operator--(int) {
			if (positiv)return decr();
			return incr();
		}
	};
}

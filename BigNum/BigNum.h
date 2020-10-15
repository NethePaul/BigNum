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
				bignum_overflow,//big num max = 32^(UINT32_MAX+1)-1
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
		bool positiv;

		
	private:
		//ignore sign{
		
		BigInt&add(const BigInt&,unsigned int P = 0);//addition
		BigInt&sub(const BigInt&);//subtraction
		BigInt&mul(const BigInt&);//multiplication
		BigInt&div(const BigInt&, BigInt*rest= 0);//division
		BigInt&incr();//increment;
		BigInt&decr();//decrement

		//}
	public:
		BigInt&mod(const BigInt&);

	public:
		friend BigInt hyper(unsigned int l, BigInt a, BigInt b);//hyper operator
		
	public:
		bool operator>(const BigInt&)const;
		bool operator>=(const BigInt&)const;
		bool operator<(const BigInt&)const;
		bool operator<=(const BigInt&)const;



		bool operator==(const BigInt&)const;
		bool operator!=(const BigInt&)const;


	public:
		friend BigInt pow(const BigInt&x,const BigInt&y);//power x^y
		friend BigInt root(const BigInt&x, const BigInt&y);//root yth root of x
		friend BigInt log(const BigInt&x, const BigInt&y);//logarithm log of x to the base of y


		friend class BigFloat;
	public:
		void setZero();
		void clear_back();
		void clear_error();
		const auto&getErrors()const { return errors; }
		std::string getNumDec()const;
		std::string getNumHex()const;
	private:
		char Hex_from_4Bit(unsigned short in)const;
	public:
		//*
		BigInt(signed long long rhs);
		BigInt(ltype rhs, bool);//apparently you can't overload a function for both (signed T) and (unsigned T) so the second parameter is only for the copiler to differentiate between both costructors 
		BigInt(const BigInt&rhs);
	public:
		BigInt&operator=(const BigInt&rhs);
		BigInt&operator+=(const BigInt&rhs);
		BigInt&operator-=(const BigInt&rhs);
		BigInt&operator*=(const BigInt&rhs);
		BigInt&operator/=(const BigInt&rhs);
		BigInt&operator%=(const BigInt&rhs);
		BigInt&operator<<=(ltype rhs);
		BigInt&operator>>=(ltype rhs);
		BigInt operator%(const BigInt&rhs)const;
		BigInt operator+(const BigInt&rhs)const;
		BigInt operator-(const BigInt&rhs)const;
		BigInt operator*(const BigInt&rhs)const;
		BigInt operator/(const BigInt&rhs)const;
		BigInt operator >> (ltype in)const;
		BigInt operator<<(ltype in)const;
		BigInt operator++(int);
		BigInt operator--(int);
	};
}

#pragma once
#include<vector>
#include<math.h>
#include<string>


namespace BigNum {
	class BigInt {
		using type = unsigned int;
		using ltype = unsigned long long;//long version of type
	public:
		BigInt&operator=	(const BigInt&rhs);			//copy assignment opertor
		BigInt&operator+=	(const BigInt&rhs);			//addition
		BigInt&operator-=	(const BigInt&rhs);			//subtraction
		BigInt&operator*=	(const BigInt&rhs);			//multiplication
		BigInt&operator/=	(const BigInt&rhs);			//division
		BigInt&operator%=	(const BigInt&rhs);			//Mod
		BigInt&operator&=	(const BigInt&rhs);			//bitwise and
		BigInt&operator|=	(const BigInt&rhs);			//bitwise or
		BigInt&operator^=	(const BigInt&rhs);			//bitwise xor
		BigInt&operator<<=	(ltype rhs);				//bitwise left shift
		BigInt&operator>>=	(ltype rhs);				//bitwise right shift
		BigInt operator%	(const BigInt&rhs)	const;	//Mod
		BigInt operator+	(const BigInt&rhs)	const;	//addition
		BigInt operator-	(const BigInt&rhs)	const;	//subtraction
		BigInt operator*	(const BigInt&rhs)	const;	//multiplication
		BigInt operator/	(const BigInt&rhs)	const;	//division
		BigInt operator&	(const BigInt&rhs)	const;	//bitwise and
		BigInt operator|	(const BigInt&rhs)	const;	//bitwise or
		BigInt operator^	(const BigInt&rhs)	const;	//bitwise xor
		BigInt operator~	()					const;	//bitwise not
		BigInt operator>>	(ltype in)			const;	//bitwise right shift
		BigInt operator<<	(ltype in)			const;	//bitwise left shift
		BigInt operator++	(int);						//increment postfix
		BigInt operator--	(int);						//decrement postfix
		BigInt operator++	();							//increment prefix
		BigInt operator--	();							//decrement prefix
	public:
		friend BigInt pow(const BigInt&x, const BigInt&y);	//power x to the power of y
		friend BigInt root(const BigInt&x, const BigInt&y);//root yth root of x
		friend BigInt log(const BigInt&x, const BigInt&y);	//logarithm log of x to the base of y
		friend BigInt hyper(unsigned long long l,const BigInt&a,const BigInt&b);//hyper operator
		friend BigInt GCD(BigInt x, BigInt y);				//greatest common divisor
		friend BigInt LCM(const BigInt&x, const BigInt&y);	//least common multiple
		friend BigInt abs(const BigInt&x);					//absolute
	public:
		bool operator>	(const BigInt&)const;
		bool operator>=	(const BigInt&)const;
		bool operator<	(const BigInt&)const;
		bool operator<=	(const BigInt&)const;
		bool operator==	(const BigInt&)const;
		bool operator!=	(const BigInt&)const;
		bool operator!()const { return *this==0; }
	public:
		void setZero();//same as BigInt::operator=(0);
		void clear_error();
		const auto&getErrors()const { return errors; }
		std::string getNumDec()const;
		std::string getNumHex()const;
	public:
		BigInt(signed long long rhs = 0);
		BigInt(ltype rhs, bool);//apparently you can't overload a function for both (signed T) and (unsigned T) so the second parameter is only for the copiler to differentiate between both costructors 
		BigInt(const BigInt&rhs);
		explicit operator bool()const { return *this != 0; };

	private:

		std::vector<type>value;
		std::vector<short>errors;
		bool positiv;

	private:

		BigInt&add(const BigInt&, ltype P = 0);//addition
		BigInt&sub(const BigInt&);//subtraction
		BigInt&mul(const BigInt&);//multiplication
		BigInt&div(const BigInt&, BigInt*rest = 0);//division
		BigInt&mod(const BigInt&);


		friend class BigFloat;
	private:
		void clear_back();
		char Hex_from_4Bit(unsigned short in)const;
	};
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
				integer_root_accuracy_loss,
				integer_log_accuracy_loss,
				//insert more non fatal errors here
				count
				
			};
		}
		enum {total_error_count = nonfatal::count};
	}

	class BigFloat;
}

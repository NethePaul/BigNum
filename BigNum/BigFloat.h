#pragma once
#include"BigNum.h"

namespace BigNum {
	class BigFloat {
		BigFloat&operator=	(const BigFloat&rhs);			//copy assignment opertor
		BigFloat&operator+=	(const BigFloat&rhs);			//addition
		BigFloat&operator-=	(const BigFloat&rhs);			//subtraction
		BigFloat&operator*=	(const BigFloat&rhs);			//multiplication
		BigFloat&operator/=	(const BigFloat&rhs);			//division
		BigFloat operator+	(const BigFloat&rhs)	const;	//addition
		BigFloat operator-	(const BigFloat&rhs)	const;	//subtraction
		BigFloat operator*	(const BigFloat&rhs)	const;	//multiplication
		BigFloat operator/	(const BigFloat&rhs)	const;	//division
		BigFloat operator++	(int);							//increment postfix
		BigFloat operator--	(int);							//decrement postfix
		BigFloat&operator++	();								//increment prefix
		BigFloat&operator--	();								//decrement prefix
	public:
		friend BigFloat pow(const BigFloat&x, const BigFloat&y);//power x to the power of y
		friend BigFloat root(const BigFloat&x,const BigFloat&y);//root yth root of x
		friend BigFloat log(const BigFloat&x, const BigFloat&y);//logarithm log of x to the base of y	//undefined
		friend BigFloat abs(const BigFloat&x);					//absolute								//undefined
	public:
		bool operator>	(const BigFloat&)const;//undefined
		bool operator>=	(const BigFloat&)const;//undefined
		bool operator<	(const BigFloat&)const;//undefined
		bool operator<=	(const BigFloat&)const;//undefined
		bool operator==	(const BigFloat&)const;//undefined
		bool operator!=	(const BigFloat&)const;//undefined
		bool operator!()const { return *this == 0; }
	public:
		void setZero() { *this = 0; };
		void clear_error() { numerator.clear_error(); denominator.clear_error(); };
		const auto&getErrors()const;//undefined
		std::string getNumDec(unsigned long long max_decimals=10)const;//the parameter determines how many decimals after the dot should be outputed if needed

		BigFloat(const BigInt&numerator, const BigInt&denominator = 1);
		BigFloat(const BigFloat&)=default;
		BigFloat(long long rhs) { *this = BigInt(rhs); }
		BigFloat(const std::string&);

		BigFloat&convertToFraction();// 4/12 -> 1/3 or 5/25 -> 1/5 or 9/3 -> 3/1
	public:
		BigInt numerator;
		BigInt denominator;


	};
}
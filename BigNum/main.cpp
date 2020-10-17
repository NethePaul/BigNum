#include"BigNum.h"
#include<iostream>

using namespace BigNum;
using namespace std;
void report_error(const BigInt&i);
int main() {
	BigInt var1 = 1000000000000000000;
	std::cout << " " << var1.getNumDec() << std::endl;
	var1 = pow(var1, 2);
	var1 <<= 0;
	std::cout <<" "<< var1.getNumDec() << std::endl;
	var1 = root(var1, 2);
	auto a = var1.getNumDec();
	std::wcout << L"=" << a.c_str() <<L"^2"<< std::endl;
	std::cout << '=' << pow(var1,2).getNumDec()<< std::endl;
	std::wcout << L" " << var1++.getNumDec().c_str() << L"^2" << std::endl;
	std::cout << '=' << pow(var1,2).getNumDec() << std::endl;
	report_error(var1);
	cin.get();
}

void report_error(const BigInt&i) {
	for (int l = i.getErrors().size() - 1; l >= 0; l--) {
		switch (i.getErrors()[l]) {
		case BigNum::Error::fatal::division_by_zero:cout << "fatal error:	division by zero" << endl; break;
		case BigNum::Error::fatal::invalid_function_call:cout << "fatal error:	unknown" << endl; break;
		case BigNum::Error::fatal::invalid_root:cout << "fatal error:	invalid root" << endl; break;
		case BigNum::Error::fatal::log_to_invalid_base:cout << "fatal error:	log to invalid base" << endl; break;
		case BigNum::Error::fatal::root_of_negativ_number:cout << "fatal error:	root of negativ number" << endl; break;

		case BigNum::Error::nonfatal::decimal_to_integer_accuracy_loss:cout << "accuracy loss:	decimal to integer conversion" << endl; break;
		case BigNum::Error::nonfatal::integer_division_accuracy_loss:cout << "accuracy loss:	integer division" << endl; break;
		case BigNum::Error::nonfatal::integer_root_accuracy_loss:cout << "accuracy loss:	integer root" << endl; break;
		default:
			cout << "unexpected error: id " << i.getErrors()[l] << endl;
		}
	}
}
#include"BigNum.h"
#include<iostream>

using namespace BigNum;
using namespace std;
void report_error(const BigInt&i);
int main() {
	BigInt var1 = 35, var2 = 15;
	auto result = LCM(var1, var2);
	std::cout << var1.getNumDec() << "%" << result.getNumDec() << "=" << (var1%result).getNumDec() << std::endl;
	std::cout << var2.getNumDec() << "%" << result.getNumDec() << "=" << (var2%result).getNumDec() << std::endl;
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
		case BigNum::Error::fatal::bignum_overflow:cout << "how did you even do this" << endl; break;


		case BigNum::Error::nonfatal::decimal_to_integer_accuracy_loss:cout << "accuracy loss:	decimal to integer conversion" << endl; break;
		case BigNum::Error::nonfatal::integer_division_accuracy_loss:cout << "accuracy loss:	integer division" << endl; break;
		case BigNum::Error::nonfatal::integer_root_accuracy_loss:cout << "accuracy loss:	integer root" << endl; break;
		case BigNum::Error::nonfatal::integer_log_accuracy_loss:cout << "accuracy loss:	integer log" << endl; break;
		default:
			cout << "unexpected error: id " << i.getErrors()[l] << endl;
		}
	}
}
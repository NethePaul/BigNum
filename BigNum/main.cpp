#include"BigNum.h"
#include<iostream>

using namespace BigNum;
using namespace std;
void report_error(const BigInt&i);
int main() {
	BigFloat a(81, 25);//one hundred thirds
	char buffer[1024];
	while (true) {
		std::cin >> buffer; std::cout << "______" << std::endl;
		a.numerator = std::string(buffer);
		std::cin >> buffer;
		a.denominator = std::string(buffer);
		a.convertToFraction();
		std::cout << a.getFractionDec() << std::endl;
		a = root(a, 4);//this will not return the correct value for irrational numbers
		std::cout << a.getNumDec() << std::endl;
	}
	//report_error(var1);
	system("pause");
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
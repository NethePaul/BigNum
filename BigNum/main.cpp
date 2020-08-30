#include"BigNum.h"
#include<iostream>

using namespace BigNum;
using namespace std;
void report_error(const BigInt&i);
int main() {
	BigInt
		i(10),
		j(100000000000000);
	i = hyper(4, i, 3);
	//j = hyper(4, j, 2);
	for (int l = 0; l < 12; l++) {
		cout << i.getNumDec().c_str() << "/" << j.getNumDec().c_str() << endl;
		i /= j;
		report_error(i);
		i.clear_error();
		cout << "=" << i.getNumDec().c_str() << endl << endl;
		
	}

	
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
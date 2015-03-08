#pragma once

#include <exception>

using namespace std;

class MessageException: public exception {
	private:
		string message;
	public:
		MessageException(string message) {
			this->message=message;
		}
		virtual const char* what() const throw() {
			return message.c_str();
		}
};
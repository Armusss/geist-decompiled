#ifndef EXCEPTION_H
#define EXCEPTION_H

// std::exception is defined in exception.cp; bad_exception's members are inline, so its vtable, RTTI
// and functions are emitted in Gecko_ExceptionPPC.cp (the only unit that throws one).
namespace std {
class exception {
public:
	exception() throw() {}
	virtual ~exception() throw() {}
	virtual const char* what() const throw() { return "exception"; }
};

class bad_exception : public exception {
public:
	bad_exception() throw() {}
	virtual ~bad_exception() throw() {}
	virtual const char* what() const throw() { return "bad_exception"; }
};
} // namespace std

using std::bad_exception;
using std::exception;

#endif /* EXCEPTION_H */

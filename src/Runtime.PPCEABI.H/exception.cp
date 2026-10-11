// Geist's C++ runtime has std::exception and a throw() operator delete in their own unit, between
// global_destructor_chain.c and NMWException.cp (0x802EABBC-0x802EAC88). With -inline deferred the
// functions are emitted in reverse source order.
extern "C" void free(void*);

namespace std {
class exception {
public:
	exception() throw() {}
	virtual ~exception() throw();
	virtual const char* what() const throw();
};

const char* exception::what() const throw() { return "exception"; }
} // namespace std

void operator delete(void* p) throw() {
	if (p) {
		free(p);
	}
}

std::exception::~exception() throw() {}

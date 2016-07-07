.PHONY: clean
CPPFLAGS = -I/usr/include/python2.7
CXXFLAGS = -g -Wall -std=c++11 -fPIC

_mk.so: py_module.o
	$(CXX) -shared -o _mk.so py_module.o
clean:
	rm -rf -- *.o *.pyc *.so

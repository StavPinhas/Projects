CXX=g++
CXXLINK=$(CXX)
CXXFLAGS= -g -Wall -std=c++11
OBJS= ip.o port.o string.o
EXEC=firewall.exe
RM=rm -rf libfirewall.so *.o *.exe

$(EXEC): libfirewall.so libinput.so main.o
	$(CXXLINK) $(CXXFLAGS)	main.o	-o	$(EXEC) -lfirewall -linput -L.

libfirewall.so: $(OBJS)
	$(CXXLINK) $(CXXFLAGS) -shared $(OBJS) -o libfirewall.so -L.

string.o: string.h string.cpp
	$(CXX)	$(CXXFLAGS)	-c	-fpic	string.cpp

port.o:	port.cpp port.h	string.h generic-field.h
	$(CXX)	$(CXXFLAGS)	-c	-fpic	port.cpp

ip.o: ip.cpp ip.h string.h generic-field.h
	$(CXX)	$(CXXFLAGS)	-c	-fpic	ip.cpp

main.o:	main.cpp port.h ip.h generic-field.h string.h
	$(CXX) $(CXXFLAGS) 	-c 	-fpic 	main.cpp 
clean:
	$(RM)

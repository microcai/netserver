
bin_PROGRAMS = netserver

netserver_SOURCES = net.cpp netserver.cpp jobqueue.hpp 

netserver_DEPENDENCIES = stdafx.h.gch

stdafx.h.gch: stdafx.h inc.h  defs.h BasePacket.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -x c++-header $< $< -o $@

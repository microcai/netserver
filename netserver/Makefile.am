
netserverdir= $(top_builddir/netserver)
PCH=stdafx.h.gch
PCH_HEADER=stdafx.h inc.h  defs.h BasePacket.h

bin_PROGRAMS = netserver

netserver_SOURCES = net.cpp netserver.cpp 
netserver_HEADERS = jobqueue.hpp  net.hpp worker.hpp stdafx.cpp stdafx.h inc.h  defs.h BasePacket.h

netserver_SOURCES: $(PCH)

$(PCH) : $(PCH_HEADER)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< 


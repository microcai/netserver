if EXTRA_GOOGLE_PROTOBUFF
AM_CXXFLAGS = 
else
AM_CXXFLAGS = -I$(top_srcdir)/protobuf
endif


netserverdir= $(top_builddir/netserver)
PCH=stdafx.h.gch
PCH_HEADER=stdafx.h inc.h  defs.h BasePacket.h

bin_PROGRAMS = netserver

netserver_SOURCES = net.cpp netserver.cpp protocol.pb.cc
netserver_HEADERS = jobqueue.hpp  net.hpp \
					worker.hpp stdafx.cpp \
					stdafx.h inc.h  defs.h \
					config.h  protocol.pb.h  stdafx.h

if EXTRA_GOOGLE_PROTOBUFF
netserver_LDADD = 
else
netserver_LDADD = -L$(top_builddir)/protobuf/.libs/
endif
					
netserver_LDADD += -lprotobuf					

netserver_SOURCES: $(PCH)

$(PCH) : $(PCH_HEADER)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< 


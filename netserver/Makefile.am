if EXTRA_GOOGLE_PROTOBUFF
AM_CXXFLAGS = 
else
AM_CXXFLAGS = -I$(top_srcdir)/protobuf -g -O0
endif


netserverdir= $(top_builddir)/netserver
PCH=stdafx.h.gch
PCH_HEADER=stdafx.h inc.h  defs.h

bin_PROGRAMS = netserver

netserver_DEPENDENCIES =  $(PCH)
netserver_SOURCES = net.cpp netserver.cpp protocol.pb.cc 
netserver_HEADERS = jobqueue.hpp  net.hpp \
					worker.hpp stdafx.cpp \
					stdafx.h inc.h  defs.h \
					config.h  protocol.pb.h  stdafx.h 

if EXTRA_GOOGLE_PROTOBUFF
netserver_LDADD = -lprotobuf
else
netserver_LDADD = $(top_builddir)/protobuf/libprotobuf.a \
					 $(top_builddir)/protobuf/libprotoc.a \
					 $(top_builddir)/protobuf/libprotobuf_lite.a
endif

#netserver_BUILD_SOURCES = $(PCH)
					
$(PCH) : $(PCH_HEADER)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(AM_CXXFLAGS)  $<
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(AM_CXXFLAGS)  $< -o $@


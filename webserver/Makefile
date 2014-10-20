CPP=clang++

EXT_PATH=$(realpath ../../external)
COMMONCPP=ip.cpp
LOCAL_PATH=$(EXT_PATH)/local
CXXFLAGS=-std=c++11 
CXXOPT=-O2 -funroll-loops -DNDEBUG -Wall
CXXDEBUG=-g -gstabs+
BOOST=-isystem$(LOCAL_PATH)/boost/include -L$(LOCAL_PATH)/boost/lib
CPPCMS=-isystem/Users/nickhathaway/source_codes/external/local/include
ZI=-I$(LOCAL_PATH)/zi_lib
LOCALTOOLS=-I$(LOCAL_PATH)
EXTTOOLS=-I$(EXT_PATH)
SRC=-I. -I../ -I../../
LDFLAGS=-Wl,-rpath,$(LOCAL_PATH)/boost/lib -L$(LOCAL_PATH)/boost/lib  \
	-lpthread -lboost_program_options -lboost_system -lboost_thread \
	-lboost_filesystem -lboost_iostreams -lboost_regex
LDFLAGS+=-Wl,-rpath,/Users/nickhathaway/source_codes/external/local/lib -L/Users/nickhathaway/source_codes/external/local/lib  \
	-lcppcms -lbooster
LDFLAGS+=-lcurl

COMMON=$(BOOST) $(ZI) $(EXTTOOLS) $(LOCALTOOLS) $(CXXFLAGS) $(CPPCMS) $(SRC) $(LDFLAGS)

.PHONY : all
all: build run
remake: all

.PHONY : build
build:
	$(CPP) webserver.cpp $(COMMONCPP) $(CXXOPT) $(COMMON)

.PHONY : debug
debug:
	$(CPP) webserver.cpp $(COMMONCPP) $(CXXDEBUG) $(COMMON)
	(echo r ; cat) | gdb ./a.out

.PHONY : run
run:
	@echo "******************** running ********************"
	./a.out
	@echo "******************** done    ********************"

.PHONY : clean
clean:
	-rm a.out

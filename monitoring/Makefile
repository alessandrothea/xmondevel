BUILD_HOME=../..

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=xdevel
Package=monitoring

Sources=\
	ClientBase.cc \
	ClientUIBase.cc \
	SimpleClient.cc \
	LASConnector.cc

IncludeDirs = \
	include\
	$(XDAQ_ROOT)/include


UserCCFlags = -g 


DependentLibraryDirs = \
	$(XDAQ_ROOT)/lib

DependentLibraries = 

DynamicLibrary= monitoring

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules


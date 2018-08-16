
#defined the home dir
project_home=/home/clodev/NetLottery


Support_Include= -I/usr/local/devsoft/include -I/usr/local/devsoft/include/libxml2/ \
		 -I/usr/local/include
#-I/usr/local/devsoft/include/zookeeper
CXX	=g++
CXXFLAGS=  -fPIC -I. -I$(project_home)/Tools -I$(project_home)/APICommon \
	-I$(project_home)/TaskCommon -I$(project_home)/CoreServiceFramework $(Support_Include)  -DNDEBUG 

RM	=rm -f 
AR	=ar

LDLIBS_DIR = -L/usr/local/devsoft/lib  -L/usr/local/lib
#-L$(project_home)/Tools
#	-L$(project_home)/APICommon -L$(project_home)/TaskCommon \
#	-L$(project_home)/CoreServiceFramework

LDLIBS_LIBS= -lc  -lcurl -lpthread -ljson -llog4cplus -luuid
#-lfcgi -lzookeeper_mt -levent_pthreads -levent -lAPICommon -lTaskCommon -lTools -lServiceFramework -lcrypto -lxml2 -ldl

.SUFFIXES:              .cpp
.cpp.o:
	$(RM) $@
	$(CXX) $(DEBUG) -c $(CXXFLAGS) -o $@ $<

SUPPORT_OBJS = $(project_home)/Tools/charset.o \
	$(project_home)/Tools/ILTools.o \
	$(project_home)/Tools/StringSplitter.o \
	$(project_home)/Tools/ILLog.o \
	$(project_home)/Tools/State.o \
	$(project_home)/Tools/CSysConf.o \
	$(project_home)/Tools/urlcode.o \
	$(project_home)/Tools/clo-rand.o \
	$(project_home)/CoreServiceFramework/ClientBase.o \
	$(project_home)/CoreServiceFramework/HttpClient.o \
	$(project_home)/CoreServiceFramework/ClientFactory.o \
	$(project_home)/CoreServiceFramework/ClientPool.o \
	$(project_home)/CoreServiceFramework/TCPClient.o

OBJS = Main.o CClient.o CCrew.o CFlow.o CGame.o CTimer.o CBetHLJN.o CBetKLSC.o CTask.o

TARGET = MultiClients 


$(TARGET):$(OBJS) $(SUPPORT_OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(SUPPORT_OBJS) $(STATIC_LIBS) $(LIB_TOOLS) $(LDLIBS_DIR) $(LDLIBS_LIBS)

#$(OBJS):%.o:%.cpp
#	$(CXX) -c $(CFLAGES) $< -o $@ 

.PHOEY: debug clean
debug:
	make DEBUG='-D__DEBUG__ -g'

clean:
	rm -f *.o *.a $(TARGET)


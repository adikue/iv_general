CXXFLAGS = -O2 -g -Wall -fmessage-length=0
#List of dependencies...
SOURCES_S = client_server.cpp app_server.cpp
SOURCES_C = client_server.cpp app_client.cpp
TARGET_S = app_server
TARGET_C = app_client
TARGETS = $TARGET_S $TARGET_S

$(TARGET_S):
	$(CXX) -o $(TARGET_S) $(SOURCES_S) $(LIBS)

$(TARGET_C):
	$(CXX) -o $(TARGET_C) $(SOURCES_C) $(LIBS)

all:	 $(TARGET_S) $(TARGET_C)

clean:
	rm -f $(TARGET_C) $(TARGET_S)

rebuild:	clean all
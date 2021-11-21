main:
	g++ -c MockFrontEnd.cpp
	g++ -o hey MockFrontEnd.o \
  	../common/out/FrontEndPayload.o \
	../app_server/out/StringExtensions.o \
	../common/out/PacketType.o -lpthread
	./hey

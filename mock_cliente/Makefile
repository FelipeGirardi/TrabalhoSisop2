listener:
	g++ -c mock_client_listener.cpp \
	../common/src/Packet.cpp \
	../common/src/PacketType.cpp \

	g++ -o listenerr mock_client_listener.o Packet.o PacketType.o
	./listenerr localhost

follower:
	g++ -c mock_client_follower.cpp \
	../common/src/Packet.cpp \
	../common/src/PacketType.cpp \

	g++ -o followerr mock_client_follower.o Packet.o PacketType.o
	./followerr localhost

sender:
	g++ -c mock_client_sender.cpp \
	../common/src/Packet.cpp \
	../common/src/PacketType.cpp \

	g++ -o senderr mock_client_sender.o Packet.o PacketType.o
	./senderr localhost

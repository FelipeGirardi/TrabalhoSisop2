SRC_DIR = src

build_server:
	g++ -std=c++11 -c MainTest.cpp src/profile_session_manager.cpp src/user_information.cpp
	g++ -o serverApp MainTest.o profile_session_manager.o user_information.o

profile_session:
	$(SRC_DIR)/profile_session_manager.cpp
	g++ -c $(SRC_DIR)/profile_session_manager.cpp

user_info:
	 $(SRC_DIR)/user_information.cpp
	 g++ -c $(SRC_DIR)/user_information.cpp

main:
	make build_server
	./serverApp
SRC_DIR = src
BUILD_DIR = build

build_server:
	g++ -std=c++11 -c MainTest.cpp src/profile_session_manager.cpp src/user_information.cpp src/utils/StringExtensions.cpp
	mv *.o $(BUILD_DIR)
	g++ -o $(BUILD_DIR)/serverApp build/*.o

profile_session:
	$(SRC_DIR)/profile_session_manager.cpp
	g++ -c $(SRC_DIR)/profile_session_manager.cpp

user_info:
	 $(SRC_DIR)/user_information.cpp
	 g++ -c $(SRC_DIR)/user_information.cpp

main:
	make build_server
	./$(BUILD_DIR)/serverApp
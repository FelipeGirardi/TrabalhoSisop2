SRC_DIR = src
BUILD_DIR = build

build_server:
	g++ -std=c++11 -c MainTest.cpp src/profile_session_manager.cpp \
	src/user_information.cpp \
	src/utils/StringExtensions.cpp \
	src/FileManager.cpp \
	../Common/src/Notification.cpp \
	src/NotificationManager.cpp \
	src/utils/ListExtensions.cpp

	mv *.o $(BUILD_DIR)
	g++ -o $(BUILD_DIR)/serverApp build/*.o -lpthread

clean:
	rm $(BUILD_DIR)/*

main:
	make build_server
	./$(BUILD_DIR)/serverApp
SDK_DIR := ./discord-sdk/
SDK_SRC_FILES := $(wildcard $(SDK_DIR)/*.cpp)
SRC_DIR := ./src/
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)


discordnvim: $(SRC_FILES) $(SDK_SRC_FILES)
	echo $(SRC_FILES)
	echo $(SDK_SRC_FILES)
	g++ $(SRC_FIELS) $(SDK_SRC_FILES) -o $@


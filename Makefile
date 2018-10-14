# Made by Robin Voetter (s1835130)

TARGET := oxybelis
SRC := src
BUILD := build
ASSETS := assets
3RDPARTY := 3rdparty
PKGS := glfw3
CXXFLAGS := -flto -I$(3RDPARTY) -I$(SRC) -I$(BUILD)/$(ASSETS) -I/usr/include/noisepp/threadpp \
	-g  -std=c++17 -Wall -Wextra -O0 -march=native \
	-DGLFW_INCLUDE_NONE \
	`pkg-config --cflags $(PKGS)`
LDFLAGS := -flto -g -ldl -lnoise `pkg-config --libs $(PKGS)` -pthread
RSRCFLAGS := -p _$(ASSETS)_ -S $(ASSETS) -n $(ASSETS) -I "core/Resource.h" -c Resource

ifeq ($(OS),Windows_NT)
    LDFLAGS += -lopengl32 -lgdi32
else
    LDFLAGS += -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor
endif

find = $(shell find $1 -type f -name $2 -print)

SRCS := $(call find, $(SRC)/, "*.cpp") $(call find, $(3RDPARTY)/, "*.cpp")

OBJECTS := $(SRCS:%.cpp=$(BUILD)/objects/%.o)
RSRCS := $(call find, $(ASSETS)/, "*")

ESC := 
RED := $(ESC)[1;31m
WHITE := $(ESC)[1;37m
BLUE := $(ESC)[1;34m
YELLOW := $(ESC)[1;33m
CLEAR := $(ESC)[0m

TOTAL := $(words $(SRCS) . . )
progress = $(or $(eval PROCESSED := $(PROCESSED) .),$(info $(WHITE)[$(YELLOW)$(words $(PROCESSED))$(WHITE)/$(YELLOW)$(TOTAL)$(WHITE)] $1$(CLEAR)))

all: $(BUILD)/target/$(TARGET)

$(BUILD)/target/$(TARGET): $(BUILD)/$(ASSETS)/$(ASSETS).o $(OBJECTS)
	@$(call progress,$(RED)Linking $@)
	@mkdir -p $(BUILD)/target
	@$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILD)/objects/%.o: %.cpp
	@$(call progress,$(BLUE)Compiling $<)
	@mkdir -p $(dir $@)
	@$(CXX) -MMD -c $(CXXFLAGS) -o $@ $<

$(BUILD)/$(ASSETS)/$(ASSETS).o: $(RSRCS)
	@$(call progress,$(BLUE)Compiling $(ASSETS))
	@mkdir -p $(BUILD)/$(ASSETS)
	@python3 ./genrsrc.py \
		$(RSRCFLAGS) \
		-o $(BUILD)/$(ASSETS)/$(ASSETS).asm $(BUILD)/$(ASSETS)/$(ASSETS).h \
		$(^:$(ASSETS)/%=%)
	@$(CXX) -x assembler -c -I$(ASSETS) -o $(BUILD)/$(ASSETS)/$(ASSETS).o $(BUILD)/$(ASSETS)/$(ASSETS).asm

clean:
	@echo Cleaning build files
	@rm -rf $(BUILD)

clean-cpp:
	@echo Cleaning c++ object files
	@rm -rf $(BUILD)/objects
	
run: all
	@$(BUILD)/target/$(TARGET)

-include build/objects/src/main.d

.PHONY: clean

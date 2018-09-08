# Made by Robin Voetter (s1835130)

TARGET := oxybelis
SRC := src
BUILD := build
RESOURCE := resource
CXXFLAGS := -I$(SRC) -std=c++17 -Wall -O3
LDFLAGS := 

find = $(shell find $1 -type f -name $2 -print)

SRCS := $(patsubst $(SRC)/%, %, $(call find, $(SRC)/, "*.cpp"))
OBJECTS := $(SRCS:%.cpp=%.o)
RSRCS := $(call find, $(RESOURCE)/, "*")

ESC := 
RED := $(ESC)[1;31m
WHITE := $(ESC)[1;37m
BLUE := $(ESC)[1;34m
YELLOW := $(ESC)[1;33m
CLEAR := $(ESC)[0m

TOTAL := $(words $(OBJECTS) . . .)
progress = $(or $(eval PROCESSED := $(PROCESSED) .),$(info $(WHITE)[$(YELLOW)$(words $(PROCESSED))$(WHITE)/$(YELLOW)$(TOTAL)$(WHITE)] $1$(CLEAR)))

vpath %.cpp $(SRC)
vpath resources.o $(BUILD)/resource
vpath %.o $(BUILD)/objects
vpath $(TARGET) $(BUILD)/target

all: $(TARGET)

$(TARGET): $(OBJECTS) resources.o
	@$(call progress,$(RED)Linking $@)
	@mkdir -p $(BUILD)/target
	@$(CXX) -o $(BUILD)/target/$@ $(OBJECTS:%=$(BUILD)/objects/%) $(BUILD)/resource/resources.o $(LDFLAGS)

%.o: %.cpp
	@$(call progress,$(BLUE)Compiling $<)
	@mkdir -p $(BUILD)/objects/$(dir $@)
	@$(CXX) -c $(CXXFLAGS) -o $(BUILD)/objects/$@ $<

resources.o: $(RSRCS)
	@$(call progress,$(BLUE)Compiling resources)
	@mkdir -p $(BUILD)/resource
	@python3 ./genrsrc.py -n resource_ -o $(BUILD)/resource/resources.asm $(^:$(RESOURCE)/%=%)
	@$(CXX) -c -I$(RESOURCE) -o $(BUILD)/resource/$@ $(BUILD)/resource/resources.asm

clean:
	@echo Cleaning build files
	@rm -rf $(BUILD)
	
run: all
	@$(BUILD)/target/$(TARGET)

.PHONY: clean

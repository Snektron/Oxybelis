# Made by Robin Voetter (s1835130)

TARGET := oxybelis
SRC := src
BUILD := build
CXXFLAGS := -I$(SRC) -std=c++14 -Wall -O3
LDFLAGS := 

rwildcard = $(foreach d, $(wildcard $1*), $(call rwildcard, $d/, $2) $(filter $(subst *, %, $2), $d))

SRCS := $(patsubst $(SRC)/%, %, $(call rwildcard, $(SRC)/, *.cpp))
OBJECTS := $(SRCS:%.cpp=%.o)

ESC := 
RED := $(ESC)[1;31m
WHITE := $(ESC)[1;37m
BLUE := $(ESC)[1;34m
YELLOW := $(ESC)[1;33m
CLEAR := $(ESC)[0m

TOTAL := $(words $(OBJECTS) .)
progress = $(or $(eval PROCESSED := $(PROCESSED) .),$(info $(WHITE)[$(YELLOW)$(words $(PROCESSED))$(WHITE)/$(YELLOW)$(TOTAL)$(WHITE)] $1$(CLEAR)))

vpath %.cpp $(SRC)
vpath %.o $(BUILD)/objects
vpath $(TARGET) $(BUILD)/target

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@$(call progress,$(RED)Linking $@)
	@mkdir -p $(BUILD)/target
	@$(CXX) -o $(BUILD)/target/$@ $(OBJECTS:%=$(BUILD)/objects/%) $(LDFLAGS)

%.o: %.cpp
	@$(call progress,$(BLUE)Compiling $<)
	@mkdir -p $(BUILD)/objects/$(dir $@)
	@$(CXX) -c $(CXXFLAGS) -o $(BUILD)/objects/$@ $<

clean:
	@echo Cleaning build files
	@rm -rf $(BUILD) $(TARGET)
	
run: all
	@$(BUILD)/target/$(TARGET)

.PHONY: clean

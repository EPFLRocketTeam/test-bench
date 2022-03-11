# directory settings
BUILD_DIR ?= build
BUILD_DIR_TEST ?= build_test
SRC_DIRS ?= src/driver src/ros src/algo

# root files with main() a function
ROOT_CXX ?= main.cc
TEST_CXX ?= main.cc


# target names
TARGET ?= out.a	
TEST_TARGET ?= test.a

# list of directories searched for include files
# ex.: '#include "foo.h"' instead of '#include "src/foo.h"'
INC_DIRS ?= $(SRC_DIRS)

# compiler settings
CXX = g++
CXXFLAGS = -g -Wall -std=c++17 -MMD -MP $(INC_FLAGS)
CXXFLAGS_TEST = -g -Wall -std=c++17 -DTESTENV -MMD -MP $(INC_FLAGS) 

LDLIBS = -lwiringPi

# --------------
# process vars
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

ROOT_OBJ := $(ROOT_CXX:%=$(BUILD_DIR)/%.o)
TEST_OBJ := $(TEST_CXX:%=$(BUILD_DIR_TEST)/%.o)

TARGET_EXEC = $(BUILD_DIR)/$(TARGET)
TEST_EXEC = $(BUILD_DIR_TEST)/$(TEST_TARGET)

# find all .cc files in src directory  
SRCS := $(shell find $(SRC_DIRS) -name "*.cc")

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
OBJS_TEST := $(SRCS:%=$(BUILD_DIR_TEST)/%.o)
DEPS := $(OBJS_:.o=.d)
DEPS_TEST := $(OBJS_TEST:.o=.d)

# --------------
# build target
$(TARGET_EXEC): $(ROOT_OBJ) $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

# run target
.PHONY: run
run: $(TARGET_EXEC)
	@echo ""
	@$<
	@echo ""

# --------------
# compile .cc files
$(BUILD_DIR)/%.cc.o: %.cc
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(BUILD_DIR_TEST)/%.cc.o: %.cc
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS_TEST) -c $< -o $@


# --------------
# build test
test: $(TEST_EXEC)


# --------------
# run test

testrun: $(TEST_EXEC)
	@echo "Run tests:\n"
	@$<
	@echo ""
	


# build test
$(TEST_EXEC): $(TEST_OBJ) $(OBJS_TEST)
	$(CXX) $(CXXFLAGS) $^ -o $@

# --------------
# clean (remove build directory)
.PHONY: clean
clean:
	@echo "\nClean build directory:"
	@$(RM) -r $(BUILD_DIR)
	@$(RM) -r $(BUILD_DIR_TEST)
	@echo "clean successful\n"

# --------------
# load all dependencies (auto generated by compiler)
-include $(DEPS) 
-include $(DEPS_TEST)

# define command to create directories (and parents), ex.: /build
MKDIR_P ?= mkdir -p


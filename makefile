product = test
# execute, dynamic_shared, static_shared
target_type = execute
define_macros =
include_dir = ../luna
# 依赖库列表,空格分开
lib = lua luna dl
# 最终产品目录:
# 注意,只是对可执行文件和动态库而言,静态库忽略此项
target_dir = .
# 源码目录,注意不会递归
src_dir_list = .
# 依赖库目录,多个目录用空格分开
lib_dir = ../luna
# 安装目录,可以写多个,也可以留空
install_dir = ../bin

CC = gcc
CXX = g++
CFLAGS =

OS := $(shell uname)

ifeq ($(OS), Linux)
CFLAGS += -DLUA_USE_LINUX
endif

ifeq ($(OS), Darwin)
CFLAGS += -DLUA_USE_MACOSX
endif

CXXFLAGS = $(CFLAGS) -Wno-invalid-offsetof -Wno-deprecated-declarations -std=c++11

#----------------- 下面部分通常不用改 --------------------------

ifeq ($(target_type), execute)
link_flags = -Wl,-rpath ./
ifeq ($(OS), Linux)
link_flags += -Wl,-E
endif
endif

ifeq ($(target_type), dynamic_shared)
CFLAGS += -fPIC
link_flags = -shared -ldl -fPIC -lpthread
ifeq ($(OS), Darwin)
link_flags += -undefined dynamic_lookup
endif
endif

ifeq ($(target_type), static_shared)
link_flags =
endif

ifeq ($(target_type), execute)
target_filename = $(product)
endif

ifeq ($(target_type), dynamic_shared)
target_filename  = lib$(product).so
endif

ifeq ($(target_type), static_shared)
target_filename  = lib$(product).a
endif

target = $(target_dir)/$(target_filename)

# exe and .so
ifneq ($(target_type), static_shared)
link = g++ -o $@ $^ $(link_flags) -m64 $(lib_dir:%=-L%) $(lib:%=-l%)
endif

# .a
ifeq ($(target_type), static_shared)
link = ar cr $@ $^ $(link_flags)
endif

the_goal = release
ifneq ($(MAKECMDGOALS),)
the_goal = $(MAKECMDGOALS)
endif

ifeq ($(the_goal),debug)
CFLAGS += -g
define_macros += _DEBUG
endif

ifeq ($(the_goal),release)
CFLAGS += -O3
endif

src_c_pattern := $(foreach node, $(src_dir_list), $(node)/*.c)
src_cpp_pattern := $(foreach node, $(src_dir_list), $(node)/*.cpp)
clear_o_pattern := $(foreach node, $(src_dir_list), $(node)/*.o)
clear_d_pattern := $(foreach node, $(src_dir_list), $(node)/*.d)
make_c_list := $(wildcard $(src_c_pattern))
make_cpp_list := $(wildcard $(src_cpp_pattern))
clear_o_list := $(wildcard $(clear_o_pattern))
clear_d_list := $(wildcard $(clear_d_pattern))
make_c2o_list := $(patsubst %.c, %.c.o, $(make_c_list))
make_cpp2o_list := $(patsubst %.cpp, %.cpp.o, $(make_cpp_list))
env_param := $(include_dir:%=-I%) $(define_macros:%=-D%)
install_target := $(install_dir:%=%/$(target_filename))

comp_c_echo = @echo gcc $< ...
comp_cxx_echo = @echo g++ $< ...

.PHONY: release
release: build_prompt $(target)

.PHONY: debug
debug: build_prompt $(target)

.PHONY: clean
clean:
	@echo rm "*.o" ...
	@rm -f $(clear_o_list)
	@echo rm "*.d" ...
	@rm -f $(clear_d_list)
	@echo rm $(target)
	@rm -f $(target)

.PHONY: install
install: $(install_dir) $(install_target)

.PHONY: build_prompt
build_prompt:
	@echo build $(product) $(the_goal) ...
	@echo cflags=$(CFLAGS) ...
	@echo c++flags=$(CXXFLAGS) ...
	@echo includes=$(include_dir)
	@echo defines=$(define_macros)
	@echo lib_dir=$(lib_dir)
	@echo libs=$(lib)

-include ${make_c2o_list:.o=.d} ${make_cpp2o_list:.o=.d}
%.c.o: %.c
	$(comp_c_echo)
	@$(CC) $(CFLAGS) $(env_param) -MM -MT $@ -MF $(@:.o=.d) $<
	@$(CC) $(CFLAGS) $(env_param) -c -o $@ $<

%.cpp.o: %.cpp
	$(comp_cxx_echo)
	@$(CXX) $(CXXFLAGS) $(env_param) -MM -MT $@ -MF $(@:.o=.d) $<
	@$(CXX) $(CXXFLAGS) $(env_param) -c -o $@ $<

$(target): $(make_c2o_list) $(make_cpp2o_list) | $(target_dir) 
	@echo link "-->" $@
	@$(link)
	$(after_link)

$(install_target): $(target)
	@echo install "-->" $@
	cp -f $(target) $@

$(target_dir):
	mkdir $(target_dir)

$(install_dir):
	mkdir $@


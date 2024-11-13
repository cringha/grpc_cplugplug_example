# tool macros
CXX := g++
CXXFLAGS := -I./3rd/spdlog -I/opt/MVS/include -I./3rd/cmdline
DBGFLAGS := -g
CCOBJFLAGS := $(CXXFLAGS) -c

#USER_LOCAL_LIB := /usr/lib/x86_64-linux-gnu
USR_LOCAL_LIB := /usr/local/lib

GRPC_STATIC_LIB := $(USR_LOCAL_LIB)/libabsl_flags_parse.a $(USR_LOCAL_LIB)/libgrpc++_reflection.a $(USR_LOCAL_LIB)/libgrpc++.a $(USR_LOCAL_LIB)/libprotobuf.a $(USR_LOCAL_LIB)/libabsl_flags_usage.a $(USR_LOCAL_LIB)/libabsl_flags_usage_internal.a $(USR_LOCAL_LIB)/libabsl_leak_check.a $(USR_LOCAL_LIB)/libabsl_die_if_null.a $(USR_LOCAL_LIB)/libabsl_log_initialize.a $(USR_LOCAL_LIB)/libutf8_validity.a $(USR_LOCAL_LIB)/libgrpc.a $(USR_LOCAL_LIB)/libabsl_statusor.a $(USR_LOCAL_LIB)/libupb_json_lib.a $(USR_LOCAL_LIB)/libupb_textformat_lib.a $(USR_LOCAL_LIB)/libupb_mini_descriptor_lib.a $(USR_LOCAL_LIB)/libupb_wire_lib.a $(USR_LOCAL_LIB)/libutf8_range_lib.a $(USR_LOCAL_LIB)/libupb_message_lib.a $(USR_LOCAL_LIB)/libupb_base_lib.a $(USR_LOCAL_LIB)/libupb_mem_lib.a $(USR_LOCAL_LIB)/libre2.a $(USR_LOCAL_LIB)/libz.a $(USR_LOCAL_LIB)/libcares.a $(USR_LOCAL_LIB)/libgpr.a $(USR_LOCAL_LIB)/libabsl_log_internal_check_op.a $(USR_LOCAL_LIB)/libabsl_flags_internal.a $(USR_LOCAL_LIB)/libabsl_flags_reflection.a $(USR_LOCAL_LIB)/libabsl_flags_config.a $(USR_LOCAL_LIB)/libabsl_flags_program_name.a $(USR_LOCAL_LIB)/libabsl_flags_private_handle_accessor.a $(USR_LOCAL_LIB)/libabsl_flags_commandlineflag.a $(USR_LOCAL_LIB)/libabsl_flags_commandlineflag_internal.a $(USR_LOCAL_LIB)/libabsl_raw_hash_set.a $(USR_LOCAL_LIB)/libabsl_hashtablez_sampler.a $(USR_LOCAL_LIB)/libabsl_log_internal_conditions.a $(USR_LOCAL_LIB)/libabsl_log_internal_message.a $(USR_LOCAL_LIB)/libabsl_log_internal_nullguard.a $(USR_LOCAL_LIB)/libabsl_examine_stack.a $(USR_LOCAL_LIB)/libabsl_log_internal_format.a $(USR_LOCAL_LIB)/libabsl_log_internal_proto.a $(USR_LOCAL_LIB)/libabsl_log_internal_log_sink_set.a $(USR_LOCAL_LIB)/libabsl_log_internal_globals.a $(USR_LOCAL_LIB)/libabsl_log_sink.a $(USR_LOCAL_LIB)/libabsl_log_entry.a $(USR_LOCAL_LIB)/libabsl_log_globals.a $(USR_LOCAL_LIB)/libabsl_hash.a $(USR_LOCAL_LIB)/libabsl_city.a $(USR_LOCAL_LIB)/libabsl_low_level_hash.a $(USR_LOCAL_LIB)/libabsl_vlog_config_internal.a $(USR_LOCAL_LIB)/libabsl_log_internal_fnmatch.a $(USR_LOCAL_LIB)/libabsl_random_distributions.a $(USR_LOCAL_LIB)/libabsl_random_seed_sequences.a $(USR_LOCAL_LIB)/libabsl_random_internal_pool_urbg.a $(USR_LOCAL_LIB)/libabsl_random_internal_randen.a $(USR_LOCAL_LIB)/libabsl_random_internal_randen_hwaes.a $(USR_LOCAL_LIB)/libabsl_random_internal_randen_hwaes_impl.a $(USR_LOCAL_LIB)/libabsl_random_internal_randen_slow.a $(USR_LOCAL_LIB)/libabsl_random_internal_platform.a $(USR_LOCAL_LIB)/libabsl_random_internal_seed_material.a $(USR_LOCAL_LIB)/libabsl_random_seed_gen_exception.a $(USR_LOCAL_LIB)/libabsl_status.a $(USR_LOCAL_LIB)/libabsl_cord.a $(USR_LOCAL_LIB)/libabsl_cordz_info.a $(USR_LOCAL_LIB)/libabsl_cord_internal.a $(USR_LOCAL_LIB)/libabsl_cordz_functions.a $(USR_LOCAL_LIB)/libabsl_exponential_biased.a $(USR_LOCAL_LIB)/libabsl_cordz_handle.a $(USR_LOCAL_LIB)/libabsl_synchronization.a $(USR_LOCAL_LIB)/libabsl_graphcycles_internal.a $(USR_LOCAL_LIB)/libabsl_kernel_timeout_internal.a $(USR_LOCAL_LIB)/libabsl_crc_cord_state.a $(USR_LOCAL_LIB)/libabsl_crc32c.a $(USR_LOCAL_LIB)/libabsl_crc_internal.a $(USR_LOCAL_LIB)/libabsl_crc_cpu_detect.a $(USR_LOCAL_LIB)/libabsl_strerror.a $(USR_LOCAL_LIB)/libabsl_stacktrace.a $(USR_LOCAL_LIB)/libabsl_symbolize.a $(USR_LOCAL_LIB)/libabsl_malloc_internal.a $(USR_LOCAL_LIB)/libabsl_debugging_internal.a $(USR_LOCAL_LIB)/libabsl_demangle_internal.a $(USR_LOCAL_LIB)/libabsl_time.a $(USR_LOCAL_LIB)/libabsl_civil_time.a $(USR_LOCAL_LIB)/libabsl_time_zone.a $(USR_LOCAL_LIB)/libabsl_bad_variant_access.a $(USR_LOCAL_LIB)/libabsl_flags_marshalling.a $(USR_LOCAL_LIB)/libabsl_bad_optional_access.a $(USR_LOCAL_LIB)/libabsl_str_format_internal.a $(USR_LOCAL_LIB)/libabsl_strings.a $(USR_LOCAL_LIB)/libabsl_strings_internal.a $(USR_LOCAL_LIB)/libabsl_string_view.a $(USR_LOCAL_LIB)/libabsl_base.a $(USR_LOCAL_LIB)/libabsl_spinlock_wait.a -lrt $(USR_LOCAL_LIB)/libabsl_int128.a $(USR_LOCAL_LIB)/libabsl_throw_delegate.a $(USR_LOCAL_LIB)/libabsl_raw_logging_internal.a $(USR_LOCAL_LIB)/libabsl_log_severity.a $(USR_LOCAL_LIB)/libssl.a $(USR_LOCAL_LIB)/libcrypto.a $(USR_LOCAL_LIB)/libaddress_sorting.a


#  -static-libgcc -static-libstdc++
LIBRARY_PATH_FLAGS := -L/opt/MVS/lib/64 -L/usr/lib/x86_64-linux-gnu -lMvCameraControl /usr/local/lib/libyaml-cpp.a $(GRPC_STATIC_LIB) -lpthread -lm -lc
# $(USER_LOCAL_LIB)/libpthread.a $(USER_LOCAL_LIB)/libm.a $(USER_LOCAL_LIB)/libc.a

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug

project_name := makefile-template

# compile macros
TARGET_NAME := camera-service
ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif
TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
                  $(OBJ_DEBUG)
CLEAN_LIST := $(TARGET) \
			  $(TARGET_DEBUG) \
			  $(DISTCLEAN_LIST)

# default rule
default: makedir all

builder-build :
	docker build -f builder.Dockerfile -t $(project_name)-builder:latest .

builder-run :
	docker run \
		--rm \
		-it \
		--platform linux/amd64 \
		--workdir /builder/mnt \
		-v ${PWD}:/builder/mnt \
		$(project_name)-builder:latest \
		/bin/bash


# non-phony targets
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LIBRARY_PATH_FLAGS) 

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CXX) $(CCOBJFLAGS) -o $@ $<

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CXX) $(CCOBJFLAGS) $(DBGFLAGS) -o $@ $<

$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(OBJ_DEBUG) -o $@

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH) $(DBG_PATH)

.PHONY: all
all: $(TARGET)

.PHONY: debug
debug: $(TARGET_DEBUG)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)

targetName=qtutils_unit_test

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

firstTarget: all

include $(mkfile_dir)/../../common/common_mkfl/flagsandsys_common_private.unix.Makefile

UNIT_TEST_SRCS_DIR=$(repoRootPath)/src/tests/unit_test
UNIT_TEST_SRCS	= $(shell find $(UNIT_TEST_SRCS_DIR) -name "*.cpp")

CPPFLAGS += -std=c++11

LIBS += -pthread

all: $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/test/$(targetName)

$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/test/$(targetName): \
			$(UNIT_TEST_SRCS:%=$(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)
	@mkdir -p $(@D)
	@echo !!!! $@
	@$(LINK) $^ $(LIBS) $(LFLAGS) -o $@

.PHONY: clean
clean:
	@rm -rf $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)
	@rm -f  $(artifactRoot)/sys/$(lsbCode)/$(Configuration)/test/$(targetName)
	@echo "  " cleaning of $(targetName) complete !!!

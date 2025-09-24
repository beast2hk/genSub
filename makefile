CURRPATH=$(shell pwd)
COMMON_FLAGS=-I${CURRPATH}/genSub -I${CURRPATH}/../Be/deploy/include -I`brew --prefix`/include `pkgconf --cflags libuv`
CPPFLAGS= -std=c++20
LDFLAGS=-L`brew --prefix`/lib `pkgconf --libs libuv` -ljson_spirit

DEPLOYPATH=deploy
BUILDPATH=build
SRCPATH=genSub

LIBRARY_NAME="libbe.a"

TARGET_SRCS=$(subst $(SRCPATH)/,,$(wildcard $(SRCPATH)/bin/*.cpp))
TARGET_OBJS=$(patsubst %.cpp,%.o,$(TARGET_SRCS))
TARGET=$(subst .cpp,,$(notdir $(wildcard $(SRCPATH)/bin/*.cpp)))

SRCS1=$(subst $(SRCPATH)/,,$(shell find $(SRCPATH) -name "*.cpp"))
SRCS2=$(subst $(SRCPATH)/,,$(wildcard $(SRCPATH)/*.mm  $(SRCPATH)/*/*.mm))
SRCS3=$(subst $(SRCPATH)/,,$(wildcard $(SRCPATH)/*.m   $(SRCPATH)/*/*.m))

SUB_DIRS=$(subst $(SRCPATH)/,,$(shell find $(SRCPATH) -type d))

OBJS1=$(filter-out $(TARGET_OBJS), $(patsubst %.cpp,%.o,$(SRCS1)))
OBJS2=$(filter-out $(TARGET_OBJS), $(patsubst %.mm,%.o,$(SRCS2)))
OBJS3=$(filter-out $(TARGET_OBJS), $(patsubst %.m,%.o,$(SRCS3)))

OBJS=${OBJS1}
OBJS+=${OBJS2}
OBJS+=${OBJS3}

export GCH
export CURRPATH
export DEPLOYPATH
export BUILDPATH
export COMMON_FLAGS
export CPPFLAGS
export LDFLAGS
export SRCPATH
export OBJS
export TARGET_OBJS
export TARGET
export LIBRARY_NAME
export CC
export CXX

all: debug

%: make.%
	mkdir -p $(BUILDPATH)/$@ $(addprefix $(BUILDPATH)/$@/, $(SUB_DIRS))
	$(MAKE) -C $(BUILDPATH)/$@ -f $(CURRPATH)/make.$@

src-clean:
	find . -name "*~" -print0 | xargs -n 1 -0 -I {} rm -f "{}"

%-clean:
	rm -r $(BUILDPATH)/$*/*

clean: src-clean
	rm -r -f build deploy

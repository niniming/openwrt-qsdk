MKDIR = mkdir -p $(@D)
SRCPATH = src
OBJPATH = obj
SRCDIR = ./

BINARY = $(OBJPATH)/nssinfo
SOURCES = $(wildcard $(SRCDIR)/src/*.c)
HEADERS = $(wildcard $(SRCDIR)/include/*.h)
OBJECTS = $(SOURCES:$(SRCDIR)/src/%.c=$(OBJPATH)/%.o)

INCLUDE += -I../lib/include
EXTRA_CFLAGS = -Wall -Werror -UENABLE_DEBUG
LDFLAGS = -lnl-genl-3 -lnl-nss -lncurses -lglib-2.0
LDLIBS  = -L../lib/obj

all: release

release: $(BINARY)

$(OBJPATH)/%.o: $(SRCPATH)/%.c $(HEADERS)
	$(MKDIR)
	@echo [CC] $@
	@$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) -o $@ $<

$(BINARY): $(OBJECTS)
	@echo $(BINARY)
	@echo [LD] $@
	@$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)
clean:
	@echo [Clean]
	@rm -f $(OBJECTS)
	@rm -f $(BINARY)
	@rmdir $(OBJPATH)

.PHONY: clean

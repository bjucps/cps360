
# dependency files
TOOLS_DEPS := tools/mkfs.d

# all generated files
TOOLS_CLEAN := tools/mkfs tools/mkfs.o $(TOOLS_DEPS)

# flags
TOOLS_CPPFLAGS := -iquote include

# mkfs
tools/mkfs: tools/mkfs.o
	$(NATIVECC) $(LDFLAGS) $< -o $@

# build object files from c files
tools/%.o: tools/%.c
	$(NATIVECC) -c $(CPPFLAGS) $(TOOLS_CPPFLAGS) $(CFLAGS) $(TOOLS_CLFAGS) -o $@ $<

# build dependency files form c files
tools/%.d: tools/%.c
	$(NATIVECC) $(CPPFLAGS) $(TOOLS_CPPFLAGS) $(CFLAGS) $(TOOLS_CFLAGS) \
	  -M -MG $< -MF $@ -MT $@ -MT $(<:.c=.o)


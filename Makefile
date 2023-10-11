CFLAGS := -Wall -pedantic -MMD -I. -Wno-strict-prototypes
LDFLAGS := -lm -lSDL2 -lSDL2_image

OUT := chmess
SRC := $(shell find . -type f -name '*.c')
OBJ := $(SRC:.c=.o)
DEP := $(SRC:.c=.d)

$(OUT): $(OBJ)
	@$(CC) $^ $(LDFLAGS) -o $@
	@echo "  LD     $@"

-include $(DEP)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "  CC     $@"

.PHONY: print-%
print-%:
	@echo "MAKEFILE: $* = $($*)"

.PHONY: clean
clean:
	rm -rf $(OUT) $(OBJ) $(DEP)

.PHONY: run
run: $(OUT)
	./$(OUT)

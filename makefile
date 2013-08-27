CC=clang

CWARN=-Wall -Wextra -fno-common -Wdeclaration-after-statement
-Wformat=2 -Winit-self -Winline -Wpacked -Wp,-D_FORTIFY_SOURCE=2 \
-Wpointer-arith -Wlarger-than-65500 -Wmissing-declarations \
-Wmissing-format-attribute -Wmissing-noreturn -Wmissing-prototypes \
-Wnested-externs -Wold-style-definition -Wredundant-decls \
-Wsign-compare -Wstrict-aliasing=2 -Wstrict-prototypes -Wswitch \
-Wundef -Wunreachable-code -Wwrite-strings -Wconversion \
-Werror=implicit-function-declaration -Wenum-compare \
-Wno-typedef-redefinition -Wparentheses\
-ansi -pedantic -pedantic-errors

# debug by default
ifdef RELEASE
CFLAGS=$(CWARN) -O3
else
CFLAGS=$(CWARN) -g -D_DEBUG
endif

CLIB=-lclang

OUTDIR=bin
SRCDIR=src

all: $(SRCDIR)/clang_dev.c
	$(CC) $(CFLAGS) -o $(OUTDIR)/clang_dev $(SRCDIR)/clang_dev.c $(CLIB)

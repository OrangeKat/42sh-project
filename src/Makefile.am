SUBDIRS = builtins \
		  lexer_parser \
		  utils

bin_PROGRAMS = 42sh

42sh_SOURCES = main.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Wall -Wextra -Werror -Wvla -pedantic

42sh_LDADD = \
	lexer_parser/liblexpar.a \
	builtins/libbin.a \
	utils/libutils.a

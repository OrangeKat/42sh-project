# Name of the libraries
lib_LIBRARIES = liblexpar.a

# List of files to compile in liblexpar.a
liblexpar_a_SOURCES= \
	ast/ast.c \
	ast/ast.h \
	lexer/lexer.c \
	lexer/lexer.h \
	lexer/token.h \
	parser/parser.c \
	parser/parser.h \
	parser/parser_utils.c \
	parser/parser_utils.h \
	eval/eval.c \
	eval/eval.h \
	eval/pipe.c \
	eval/redir.c

liblexpar_a_CPPFLAGS = -I$(top_srcdir)/src
liblexpar_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = liblexpar.a

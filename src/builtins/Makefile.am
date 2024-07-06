# Name of the libraries
lib_LIBRARIES = libbin.a

# List of files to compile in libbin.a
libbin_a_SOURCES= \
	echo.c \
	true.c \
	false.c \
	exit.c \
	cd.c \
	builtin.h

libbin_a_CPPFLAGS = -I$(top_srcdir)/src
libbin_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libbin.a

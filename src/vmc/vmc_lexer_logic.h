#ifndef _VM_LEXER_LOGIC_H_
#define _VM_LEXER_LOGIC_H_

#include "../vm_config.h"

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

static inline BOOL vmc_lexer_test_keyword(vm_byte c)
{
	switch (c) {
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
		return TRUE;
	default:
		return FALSE;
	}
}

static inline BOOL vmc_lexer_test_uppercase(vm_byte c)
{
	switch (c) {
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
		return TRUE;
	default:
		return FALSE;
	}
}

static inline BOOL vmc_lexer_test_number(vm_byte c)
{
	switch (c) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return TRUE;
	default:
		return FALSE;
	}
}

static inline BOOL vml_test_hex(vm_byte c)
{
	switch (c) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return TRUE;
	default:
		return FALSE;
	}
}

static inline BOOL vmc_lexer_test_char(vm_byte c)
{
	return vmc_lexer_test_keyword(c) || vmc_lexer_test_number(c);
}

// Test if rune is a whitespace. Newlines are not considered white-space in this function
static inline BOOL vmc_lexer_test_whitespace_ignore_nl(vm_byte c)
{
	switch (c) {
	case ' ':
	case '\t':
	case '\r':
		return TRUE;
	default:
		return FALSE;
	}
}

// Test if rune is a whitespace
static inline BOOL vmc_lexer_test_whitespace(vm_byte c)
{
	switch (c) {
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		return TRUE;
	default:
		return FALSE;
	}
}

static inline BOOL vml_test_newline(vm_byte c)
{
	return c == '\n';
}

// Check to see if the supplied byte is escapable
static inline BOOL vmc_lexer_test_escapeable(vm_byte c)
{
	switch (c)
	{
	case '\\':
	case 't':
	case 'n':
	case 'r':
	case '0':
	case '\'':
	case '"':
	case '`':
		return TRUE;
	default:
		return FALSE;
	}
}

#endif

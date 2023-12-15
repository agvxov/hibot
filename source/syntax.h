#include <string.h>

#define SYNTAX_LIMIT (144)

#define EFFECT_NORMAL    ('0')
#define EFFECT_BOLD      ('1')
#define EFFECT_ITALIC    ('3')
#define EFFECT_UNDERLINE ('4')
#define EFFECT_BLINK     ('5')
#define EFFECT_REVERSE   ('7')

#define COLOUR_GREY   ('0')
#define COLOUR_RED    ('1')
#define COLOUR_GREEN  ('2')
#define COLOUR_YELLOW ('3')
#define COLOUR_BLUE   ('4')
#define COLOUR_PINK   ('5')
#define COLOUR_CYAN   ('6')
#define COLOUR_WHITE  ('7')

extern void syntax_c   (void);
extern void syntax_ada (void);

extern char * syntax_highlight (char * string);

static size_t syntax_count = 0;

static int  syntax_enrange [SYNTAX_LIMIT];
static int  syntax_derange [SYNTAX_LIMIT];
static char syntax_begin   [SYNTAX_LIMIT] [96];
static char syntax_end     [SYNTAX_LIMIT] [96];
static char syntax_escape  [SYNTAX_LIMIT];
static char syntax_colour  [SYNTAX_LIMIT];
static char syntax_effect  [SYNTAX_LIMIT];

static int character_compare_array (char character, char * character_array) {
	size_t i = 0;

	do {
		if (character == character_array [i]) {
			return (1);
		}
	} while (++i != strlen (character_array));

	return (0);
}

static void syntax_rule (int    enrange,
                         int    derange,
                         char * begin,
                         char * end,
                         char   escape,
                         char   colour,
                         char   effect) {
	if (syntax_count >= SYNTAX_LIMIT) {
		return;
	}

	strncpy (syntax_begin [syntax_count], begin, 96);
	strncpy (syntax_end   [syntax_count], end,   96);

	syntax_enrange [syntax_count] = enrange;
	syntax_derange [syntax_count] = derange;
	syntax_escape  [syntax_count] = escape;
	syntax_colour  [syntax_count] = colour;
	syntax_effect  [syntax_count] = effect;

	++syntax_count;
}

static size_t syntax_loop (char   * string,
                           size_t * length) {
	size_t offset, subset, select;

	for (select = offset = 0; select != syntax_count; ++select) {
		if (syntax_enrange [select] == 0) {
			if (syntax_derange [select] == 0) {
				if (strncmp (string, syntax_begin [select], strlen (syntax_begin [select])) == 0) {
					break;
				}
			} else {
				if ((strncmp (string, syntax_begin [select], strlen (syntax_begin [select])) == 0)
				&&  (character_compare_array (string [offset + strlen (syntax_begin [select])], syntax_end [select]) == 1)) {
					break;
				}
			}
		} else {
			for (subset = 0; subset != strlen (syntax_begin [select]); ++subset) {
				if (string [offset] == syntax_begin [select] [subset]) {
					goto selected;
				}
			}
		}
	}

	selected:

	if (select >= syntax_count) {
		* length = 1;
		return (select);
	}

	for (offset = 1; string [offset - 1] != '\0'; ++offset) {
		if (string [offset] == syntax_escape [select]) {
			++offset;
			continue;
		}

		if (syntax_derange [select] == 0) {
			if (strncmp (& string [offset], syntax_end [select], strlen (syntax_end [select])) == 0) {
				* length = offset + strlen (syntax_end [select]);
				goto finished;
			}
		} else {
			subset = 0;
			if (strcmp (syntax_end [select], "") == 0) {
				break;
			} do {
				if (string [offset] == syntax_end [select] [subset]) {
					* length = offset;
					goto finished;
				}
			} while (++subset != strlen (syntax_end [select]));
		}
	}

	finished:

	return (select);
}

void syntax_c (void) {
	char * separators = ".,:;<=>+-*/%!&~^?|()[]{}'\" \t\r\n";

	char * keywords [] = {
		"register",         "volatile",         "auto",             "const",            "static",           "extern",           "if",               "else",
		"do",               "while",            "for",              "continue",         "switch",           "case",             "default",          "break",
		"enum",             "union",            "struct",           "typedef",          "goto",             "void",             "return",           "sizeof",
		"char",             "short",            "int",              "long",             "signed",           "unsigned",         "float",            "double"
	};

	size_t word;

	syntax_rule (0, 0, "/*", "*/", '\0', COLOUR_GREY,   EFFECT_BOLD);
	syntax_rule (0, 0, "//", "\n", '\0', COLOUR_GREY,   EFFECT_BOLD);
	syntax_rule (0, 0, "#",  "\n", '\\', COLOUR_YELLOW, EFFECT_ITALIC);
	syntax_rule (0, 0, "'",  "'",  '\\', COLOUR_PINK,   EFFECT_BOLD);
	syntax_rule (0, 0, "\"", "\"", '\\', COLOUR_PINK,   EFFECT_NORMAL);

	for (word = 0; word != sizeof (keywords) / sizeof (keywords [0]); ++word) {
		syntax_rule (0, 1, keywords [word], separators, '\0', COLOUR_YELLOW, EFFECT_BOLD);
	}

	syntax_rule (1, 0, "()[]{}",             "", '\0', COLOUR_BLUE, EFFECT_NORMAL);
	syntax_rule (1, 0, ".,:;<=>+*-/%!&~^?|", "", '\0', COLOUR_CYAN, EFFECT_NORMAL);

	syntax_rule (1, 1, "0123456789",                 separators, '\0', COLOUR_PINK,  EFFECT_BOLD);
	syntax_rule (1, 1, "abcdefghijklmnopqrstuvwxyz", separators, '\0', COLOUR_WHITE, EFFECT_NORMAL);
	syntax_rule (1, 1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", separators, '\0', COLOUR_WHITE, EFFECT_BOLD);
	syntax_rule (1, 1, "_",                          separators, '\0', COLOUR_WHITE, EFFECT_ITALIC);
}

void syntax_ada (void) {
	char * separators = ".,:;<=>+-*/&|()\" \t\r\n";

	char * keywords [] = {
		"abort",            "else",             "new",              "return",           "abs",              "elsif",            "not",              "reverse",
		"abstract",         "end",              "null",             "accept",           "entry",            "select",           "access",           "of",
		"separate",         "aliased",          "exit",             "or",               "some",             "all",              "others",           "subtype",
		"and",              "for",              "out",              "array",            "function",         "at",               "tagged",           "generic",
		"package",          "task",             "begin",            "goto",             "pragma",           "body",             "private",          "then",
		"type",             "case",             "in",               "constant",         "until",            "is",               "raise",            "use",
		"if",               "declare",          "range",            "delay",            "limited",          "record",           "when",             "delta",
		"loop",             "rem",              "while",            "digits",           "renames",          "with",             "do",               "mod",
		"requeue",          "xor",              "procedure",        "protected",        "interface",        "synchronized",     "exception",        "overriding",
		"terminate"
	};

	size_t word;

	syntax_rule (0, 0, "--", "\n", '\0', COLOUR_GREY, EFFECT_BOLD);
	syntax_rule (0, 0, "'",  "'",  '\\', COLOUR_PINK, EFFECT_BOLD);
	syntax_rule (0, 0, "\"", "\"", '\\', COLOUR_PINK, EFFECT_NORMAL);

	for (word = 0; word != sizeof (keywords) / sizeof (keywords [0]); ++word) {
		syntax_rule (0, 1, keywords [word], separators, '\0', COLOUR_YELLOW, EFFECT_BOLD);
	}

	syntax_rule (1, 0, "()",             "", '\0', COLOUR_BLUE, EFFECT_NORMAL);
	syntax_rule (1, 0, ".,:;<=>+-*/&|'", "", '\0', COLOUR_CYAN, EFFECT_NORMAL);

	syntax_rule (1, 1, "0123456789",                 separators, '\0', COLOUR_PINK,  EFFECT_BOLD);
	syntax_rule (1, 1, "abcdefghijklmnopqrstuvwxyz", separators, '\0', COLOUR_WHITE, EFFECT_NORMAL);
	syntax_rule (1, 1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", separators, '\0', COLOUR_WHITE, EFFECT_BOLD);
}

char * syntax_highlight (char * code) {
	static char buffer [4096] = "";
	static char string [4096] = "";

	size_t select, length, offset;

	memset (buffer, 0, sizeof (buffer));
	memset (string, 0, sizeof (string));

	strcpy (string, code);

	for (offset = 0; offset < strlen (string); offset += length) {
		select = syntax_loop (& string [offset], & length);

		if (select >= syntax_count) {
			strncat (buffer, "\033[1;31m", 7);
		} else {
			char colour [8] = "\033[-;3-m";
			colour [2] = syntax_effect [select];
			colour [5] = syntax_colour [select];
			strncat (buffer, colour, 7);
		}

		strncat (buffer, & string [offset], (size_t) length);

		strncat (buffer, "\033[0m", 4);
	}

	return (buffer);
}

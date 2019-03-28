#include"false.hpp"

#include<cstdlib>
#include<cstdio>
#include<cerrno>
#include<cstring>

#include<mmvi.hpp>
#include<argtable3.h>
#include<fmt/format.h>
#include<fmt/ostream.h>

int main(int argc, char **argv) {
	struct arg_lit *help = arg_lit0(NULL, "help", "print this help and exit");
	struct arg_lit *version = arg_lit0(NULL, "version", "print version information and exit");
	struct arg_str *anything = arg_strn(NULL, NULL, "ANYTHING", 0, 1000, NULL);
	struct arg_end *end = arg_end(20);
	void *argtable[] = {help, version, anything, end };
	int nerrors;

	// Verify the argtable[] entries were allocated sucessfully
	if (arg_nullcheck(argtable) != 0) {
		// NULL entries were detected, some allocations must have failed
		fmt::print("{}: insufficient memory\n", program_name);
		//deallocate each non-null entry in argtable[]
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		// Program will always exit with a status code indicating failure
		return EXIT_FAILURE;
	}

	// Parse the command line as defined by argtable[]
	nerrors = arg_parse(argc, argv, argtable);

	// special case: '--help' takes precedence over error reporting
	if (help->count > 0) {
		fmt::print("Usage: {}", program_name);
		arg_print_syntax(stdout, argtable, "\n");
		fmt::print("Exit with a status code indicating failure.\n\n");
		arg_print_glossary(stdout, argtable, "  %-20s %s\n");
		fmt::print("\nCopyright (C) 2019 Julius Behrens. All rights reserved.\n");

		//deallocate each non-null entry in argtable[]
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		// Program will always exit with a status code indicating failure
		return EXIT_FAILURE;
	}

	// special case: '--version' takes precedence error reporting
	if (version->count > 0) {
		fmt::print("{} {}.{}\n", program_name, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR);
		fmt::print("Copyright (C) 2019 Julius Behrens. All rights reserved.\n");
		//deallocate each non-null entry in argtable[]
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		// Program will always exit with a status code indicating failure
		return EXIT_FAILURE;
	}

	// If the parser returned any errors then display them and exit
	if (nerrors > 0) {
		// Display the error details contained in the arg_end struct.
		arg_print_errors(stderr, end, program_name.c_str());
		fmt::print(stderr, "Try '{} --help' for more information.\n", program_name);
		//deallocate each non-null entry in argtable[]
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		// Program will always exit with a status code indicating failure
		return EXIT_FAILURE;
	}

	//deallocate each non-null entry in argtable[]
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return EXIT_FAILURE;
}

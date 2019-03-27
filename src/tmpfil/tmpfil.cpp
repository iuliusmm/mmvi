#include"tmpfil.hpp"

#include<cstdlib>
#include<cstdio>
#include<cerrno>
#include<cstring>
#include<string>
#include<fstream>
#include<filesystem>

#include<mmvi.hpp>
#include<argtable3.h>
#include<fmt/format.h>
#include<fmt/ostream.h>

exit_code_t create_temporary_file() {
	exit_code_t exit_code = EXIT_SUCCESS;;
	std::string temporary_file_name = std::tmpnam(nullptr);
	std::ofstream temporary_file;
	temporary_file.open(temporary_file_name, std::ios::out, std::ios::trunc);
	temporary_file << "";
		if (!temporary_file.good()) {
			fmt::print(stderr, "{}: cannot create temporary file\n", program_name);
			exit_code = EXIT_FAILURE;
		}
		else {
			fmt::print("{}\n", temporary_file_name);
		}
		temporary_file.close();
	return exit_code;
}

int main(int argc, char **argv) {
	struct arg_lit  *help = arg_lit0(NULL, "help", "print this help and exit");
	struct arg_lit  *version = arg_lit0(NULL, "version", "print version information and exit");
	struct arg_end  *end = arg_end(20);
	void *argtable[] = {help, version, end };
	int nerrors;

	// Verify the argtable[] entries were allocated sucessfully
	if (arg_nullcheck(argtable) != 0) {
		// NULL entries were detected, some allocations must have failed
		fmt::print("{}: insufficient memory\n", program_name);
		//deallocate each non-null entry in argtable[]
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return EXIT_FAILURE;
	}

	// Parse the command line as defined by argtable[]
	nerrors = arg_parse(argc, argv, argtable);

	// special case: '--help' takes precedence over error reporting
	if (help->count > 0) {
		fmt::print("Usage: {}", program_name);
		arg_print_syntax(stdout, argtable, "\n");
		fmt::print("Creates a temporary file with a unique auto-generated filename.\n\n");
		arg_print_glossary(stdout, argtable, "  %-20s %s\n");
		fmt::print("\nCopyright (C) 2019 Julius Behrens. All rights reserved.\n");

		//deallocate each non-null entry in argtable[]
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return EXIT_SUCCESS;
	}

	// special case: '--version' takes precedence error reporting
	if (version->count > 0) {
		fmt::print("{} {}.{}\n", program_name, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR);
		fmt::print("Copyright (C) 2019 Julius Behrens. All rights reserved.\n");
		//deallocate each non-null entry in argtable[]
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return EXIT_SUCCESS;
	}

	// If the parser returned any errors then display them and exit
	if (nerrors > 0) {
		// Display the error details contained in the arg_end struct.
		arg_print_errors(stderr, end, program_name.c_str());
		fmt::print(stderr, "Try '{} --help' for more information.\n", program_name);
		//deallocate each non-null entry in argtable[]
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return EXIT_FAILURE;
	}

	exit_code_t exit_code = create_temporary_file();

	//deallocate each non-null entry in argtable[]
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return exit_code;
}

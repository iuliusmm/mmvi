#include"newdir.hpp"

#include<cstdlib>
#include<cerrno>
#include<cstring>
#include<system_error>
#include<filesystem>
#include<experimental/filesystem>

#include<mmvi.hpp>
#include<argtable3.h>
#include<fmt/format.h>
#include<fmt/ostream.h>

exit_code_t create_directory(int parents, int verbose, const char **directory, int ndirectory) {
	exit_code_t exit_code = EXIT_SUCCESS;;
	for (int i = 0; i < ndirectory; ++i) {
		if (std::filesystem::exists(directory[i])) {
			fmt::print(stderr, "{}: cannot create directory '{}': File exists\n", program_name, directory[i]);
			exit_code = EXIT_FAILURE;
			continue;
		}
		if (parents > 0) {
			std::error_code error_code;
			if (!std::filesystem::create_directories(directory[i], error_code)) {
				fmt::print(stderr, "{}: cannot create directory '{}': {}\n", program_name, directory[i], error_code.message());
				exit_code = EXIT_FAILURE;
				continue;
			}
		}
		else {
			std::error_code error_code;
			if (!std::filesystem::create_directory(directory[i], error_code)) {
				fmt::print(stderr, "{}: cannot create directory '{}': {}\n", program_name, directory[i], error_code.message());
				exit_code = EXIT_FAILURE;
				continue;
			}
		}
		if (verbose > 0) {
			fmt::print("{}: created directory '{}'\n", program_name, directory[i]);
		}
	}
	return exit_code;
}

int main(int argc, char **argv) {
	//struct arg_lit  *overwrite = arg_lit0("o", "overwrite", "overwrite existing directories");
	struct arg_lit  *parents = arg_lit0("p", "parents", "make parent directories as needed");
	struct arg_lit  *verbose = arg_lit0("v", "verbose", "print a message for each created directory");
	struct arg_lit  *help = arg_lit0(NULL, "help", "print this help and exit");
	struct arg_lit  *version = arg_lit0(NULL, "version", "print version information and exit");
	struct arg_file *directory = arg_filen(NULL, NULL, "DIRECTORY", 1, 1000, NULL);
	struct arg_end  *end = arg_end(20);
	void *argtable[] = { parents, verbose, help, version, directory, end };
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
		fmt::print("Create the DIRECTORY(ies).\n\n");
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

	exit_code_t exit_code = create_directory(parents->count, verbose->count, directory->filename, directory->count);

	//deallocate each non-null entry in argtable[]
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return exit_code;
}

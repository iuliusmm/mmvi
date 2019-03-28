#include"move.hpp"

#include<cstdlib>
#include<cerrno>
#include<cstring>
#include<system_error>
#include<filesystem>

#include<mmvi.hpp>
#include<argtable3.h>
#include<fmt/format.h>
#include<fmt/ostream.h>

exit_code_t move(int verbose, const char *source, const char *destination) {
	exit_code_t exit_code = EXIT_SUCCESS;
	if(!std::filesystem::exists(source)) {
		fmt::print(stderr, "{}: cannot move source '{}': No such file or directory\n", program_name, source);
		return EXIT_FAILURE;
	}

	if (std::filesystem::exists(destination) && !std::filesystem::is_directory(destination)) {
		fmt::print(stderr, "{}: cannot move to destination '{}': File exists\n", program_name, destination);
		return EXIT_FAILURE;
	}

	if (std::filesystem::exists(destination) && std::filesystem::is_directory(destination)) {
		std::error_code error_code;
		std::filesystem::rename(source, destination / std::filesystem::path(source).filename(), error_code);
		if (error_code) {
			fmt::print(stderr, "{}: cannot move '{}' to '{}': {}", program_name, source, destination, error_code.message());
			exit_code = EXIT_FAILURE;
		}

		if (verbose > 0) {
			fmt::print("{}: renamed or moved '{}' to '{}'\n", program_name, source, destination);
		}
		return EXIT_SUCCESS;
	}

	std::error_code error_code;
	std::filesystem::rename(source, destination, error_code);
	if (error_code) {
		fmt::print(stderr, "{}: cannot move '{}' to '{}': {}", program_name, source, destination, error_code.message());
		exit_code = EXIT_FAILURE;
	}
	
	if (verbose > 0) {
		fmt::print("{}: renamed or moved '{}' to '{}'", program_name, source, destination);
	}
	return exit_code;
}

int main(int argc, char **argv) {
	struct arg_lit  *verbose = arg_lit0("v", "verbose", "print a message for each renamed or moved filesystem object");
	struct arg_lit  *help = arg_lit0(NULL, "help", "print this help and exit");
	struct arg_lit  *version = arg_lit0(NULL, "version", "print version information and exit");
	struct arg_file *source = arg_filen(NULL, NULL, "SOURCE", 1, 1, NULL);
	struct arg_file *destination = arg_filen(NULL, NULL, "DESTINATION", 1, 1, NULL);
	struct arg_end  *end = arg_end(20);
	void *argtable[] = { verbose, help, version, source, destination, end };
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
		fmt::print("Rename SOURCE to DESTINATION, or move SOURCE(s) to DESTINATION.\n\n");
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

	exit_code_t exit_code = move(verbose->count, (source->filename)[0], (destination->filename)[0]);

	//deallocate each non-null entry in argtable[]
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return exit_code;
}

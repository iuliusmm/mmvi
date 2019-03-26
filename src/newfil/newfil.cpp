#include"newfil.hpp"

#include<cstdlib>
#include<fstream>
#include<filesystem>
#include<experimental/filesystem>

#include<mmvi.hpp>
#include<argtable3.h>
#include<fmt/format.h>
#include<fmt/ostream.h>

#include <cerrno>
#include <cstdio>

int create_file(int overwrite, int verbose, const char **file, int nfile) {
	int exitcode = EXIT_SUCCESS;
	std::ofstream f;
	for (int i = 0; i < nfile; ++i) {
		if (overwrite > 0) {
			f.open(file[i], std::ios::out, std::ios::trunc);
			f << "";
			if (!f.good()) {
				fmt::print(stderr, "{}: cannot create file '{}': {}\n", program_name, file[i], strerror(errno));
				exitcode = EXIT_FAILURE;
				continue;
			}
			f.close();

		}
		else {
			if (std::experimental::filesystem::exists(file[i])) {
				fmt::print(stderr, "{}: cannot create file '{}': File exists\n", program_name, file[i]);
				exitcode = EXIT_FAILURE;
				continue;
			}
			f.open(file[i], std::ios::out);
			f << "";
			if (!f.good()) {
				fmt::print(stderr, "{}: cannot create file '{}': {}\n", program_name, file[i], strerror(errno));
				exitcode = EXIT_FAILURE;
				continue;
			}
			f.close();
		}
		if (verbose) {
			fmt::print("{}: created file '{}'\n", program_name, file[i]);
		}
	}
	return exitcode;
}

int main(int argc, char **argv) {
	struct arg_lit  *overwrite = arg_lit0("o", "overwrite", "overwrite existing files");
	struct arg_lit  *verbose = arg_lit0("v", "verbose", "print a message for each created file");
	struct arg_lit  *help = arg_lit0(NULL, "help", "print this help and exit");
	struct arg_lit  *version = arg_lit0(NULL, "version", "print version information and exit");
	struct arg_file *file = arg_filen(NULL, NULL, "FILE", 1, 1000, NULL);
	struct arg_end  *end = arg_end(20);
	void *argtable[] = { overwrite, verbose, help, version, file, end };
	int exitcode = EXIT_SUCCESS;
	int nerrors;

	// Verify the argtable[] entries were allocated sucessfully
	if (arg_nullcheck(argtable) != 0) {
		// NULL entries were detected, some allocations must have failed
		fmt::print("{}: insufficient memory\n", program_name);
		exitcode = EXIT_FAILURE;
		goto exit;
	}

	// Parse the command line as defined by argtable[]
	nerrors = arg_parse(argc, argv, argtable);

	// special case: '--help' takes precedence over error reporting
	if (help->count > 0) {
		fmt::print("Usage: {}", program_name);
		arg_print_syntax(stdout, argtable, "\n");
		fmt::print("Create the FILE(s).\n\n");
		arg_print_glossary(stdout, argtable, "  %-20s %s\n");
		fmt::print("\nCopyright (C) 2019 Julius Behrens. All rights reserved.\n");

		exitcode = EXIT_SUCCESS;
		goto exit;
	}

	// special case: '--version' takes precedence error reporting
	if (version->count > 0) {
		fmt::print("{} {}.{}\n", program_name, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR);
		fmt::print("Copyright (C) 2019 Julius Behrens. All rights reserved.\n");
		exitcode = EXIT_SUCCESS;
		goto exit;
	}

	// If the parser returned any errors then display them and exit
	if (nerrors > 0) {
		// Display the error details contained in the arg_end struct.
		arg_print_errors(stderr, end, program_name.c_str());
		fmt::print(stderr, "Try '{} --help' for more information.\n", program_name);
		exitcode = EXIT_FAILURE;
		goto exit;
	}

	exitcode = create_file(overwrite->count, verbose->count, file->filename, file->count);

	exit:
	//deallocate each non-null entry in argtable[]
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return exitcode;
}
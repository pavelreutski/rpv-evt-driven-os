#pragma once

void _kernel_exec_f(
		int (*exec_func)(const int argc, const char **argv),
		const int argc, const char **argv);
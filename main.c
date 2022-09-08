#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "libGoReSym.h"

#define LIB_GO_RE_SYM_SO "/root/libGoReSym/libGoReSym.so"

int function_address(const char *file_name, const char *symbol_name, long *addr,
		     long *size)
{
	int error = 0;
	void *dlfile = NULL;
	FunctionAddress func = NULL;
	FunctionAddress_return retval = {};
	GoString fileName = {};
	GoString symbolName = {};

	if (access(LIB_GO_RE_SYM_SO, R_OK) < 0) {
		error = -ENOENT;
		goto out;
	}

	dlfile = dlopen(LIB_GO_RE_SYM_SO, RTLD_LAZY);
	if (!dlfile) {
		error = -ENOENT;
		goto out;
	}

	func = dlsym(dlfile, "FunctionAddress");
	if (!func) {
		error = -ENOENT;
		goto out;
	}

	fileName.p = file_name;
	fileName.n = strlen(file_name);

	symbolName.p = symbol_name;
	symbolName.n = strlen(symbol_name);

	retval = func(fileName, symbolName);
	if (!retval.addr) {
		error = -ENOENT;
		goto out;
	}

	*addr = retval.addr;
	*size = retval.size;
out:
	dlclose(dlfile);
	return error;
}

int itab_address(const char *file_name, const char *symbol_name, long *addr)
{
	int error = 0;
	void *dlfile = NULL;
	ITabAddress func = NULL;
	GoUintptr retval = 0;
	GoString fileName = {};
	GoString symbolName = {};

	if (access(LIB_GO_RE_SYM_SO, R_OK) < 0) {
		error = -ENOENT;
		goto out;
	}

	dlfile = dlopen(LIB_GO_RE_SYM_SO, RTLD_LAZY);
	if (!dlfile) {
		error = -ENOENT;
		goto out;
	}

	func = dlsym(dlfile, "ITabAddress");
	if (!func) {
		error = -ENOENT;
		goto out;
	}

	fileName.p = file_name;
	fileName.n = strlen(file_name);

	symbolName.p = symbol_name;
	symbolName.n = strlen(symbol_name);

	retval = func(fileName, symbolName);
	if (!retval) {
		error = -ENOENT;
		goto out;
	}

	*addr = retval;
out:
	dlclose(dlfile);
	return error;
}

int main(int argc, char *argv[])
{
	long addr = 0;
	long size = 0;

	char *file_name = "GoReSym";
	char *func_name = "runtime.casgstatus";
	char *itab_name = "go.itab.syscall.Errno,error";

	if (!function_address(file_name, func_name, &addr, &size)) {
		printf("addr=[%p], size=[%d]\n", addr, size);
	}

	if (!itab_address(file_name, itab_name, &addr)) {
		printf("addr=[%p]\n", addr);
	}

	return 0;
}
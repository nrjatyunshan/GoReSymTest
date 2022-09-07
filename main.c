#include <stdio.h>
#include <dlfcn.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include "cgo.h"

struct GoSymToAddrRet {
	GoUintptr addr;
	GoInt size;
};

int symbol_to_address(const char *file_name, const char *symbol_name,
		      long *addr, long *size)
{
	int error = 0;
	void *dlfile = NULL;
	struct GoSymToAddrRet (*SymbolToAddress)(GoString, GoString) = NULL;
	struct GoSymToAddrRet Ret = {};
	GoString fileName = {};
	GoString symbolName = {};

	dlfile = dlopen("./libGoReSym.so", RTLD_LAZY);
	if (!dlfile) {
		error = -ENOENT;
		goto out;
	}

	SymbolToAddress = dlsym(dlfile, "SymbolToAddress");
	if (!SymbolToAddress) {
		error = -ENOENT;
		goto out;
	}

	fileName.p = file_name;
	fileName.n = strlen(file_name);

	symbolName.p = symbol_name;
	symbolName.n = strlen(symbol_name);

	Ret = SymbolToAddress(fileName, symbolName);
	if (!Ret.addr) {
		error = -ENOENT;
		goto out;
	}

	*addr = Ret.addr;
	*size = Ret.size;
out:
	dlclose(dlfile);
	return error;
}

int main(int argc, char *argv[])
{
	long addr;
	long size;

	if (argc < 2) {
		printf("Usage:\n\t%s fileName symbolName\n", argv[0]);
		return 0;
	}

	if (symbol_to_address(argv[1], argv[2], &addr, &size)) {
		printf("find %s in %s failed\n", argv[2], argv[1]);
		return 0;
	}

	printf("addr=[%p], size=[%d]\n", addr, size);
	return 0;
}
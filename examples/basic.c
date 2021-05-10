#include "../ezargs.h"
#include <stdio.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

enum ArgID {
	ArgID_HELP,
	ArgID_VERSION,
	ArgID_FILE,
};

int main(int argc, char** argv) {
	if (!argc) {
		printf("no args!\n");
		return 0;
	}

	const struct ArgsMeta metas[] = {
		{
			.key = "help",
			.id = ArgID_HELP,
			.type = ArgsMetaType_NO_VALUE,
			.single = 'h'
		},
		{
			.key = "version",
			.id = ArgID_VERSION,
			.type = ArgsMetaType_NO_VALUE,
			.single = 'v'
		},
		{
			.key = "file",
			.id = ArgID_FILE,
			.type = ArgsMetaType_VALUE_REQUIRED,
			.single = 'f'
		},
	};

	struct Argsdata data[ARRAY_SIZE(metas)] = {0};
	unsigned data_count = {0};

	const enum ArgsResult result = args_parse(
		1, argc, argv,
		metas, ARRAY_SIZE(metas),
		data, &data_count
	);

	switch (result) {
		case ArgsResult_MISSING_REQUIRED_VALUE:
			printf("Args Error: ArgsResult_MISSING_REQUIRED_VALUE\n");
			return -1;

		case ArgsResult_ERROR:
			printf("Args Error: ArgsResult_ERROR\n");
			return -1;

		case ArgsResult_OK:
			break;

		case ArgsResult_EXTRA_ARGS:
			break;
	}

	for (unsigned i = 0; i < data_count; ++i) {
		switch (data[i].id) {
			case ArgID_HELP:
				printf("key: %.*s\n", data[i].key_len, data[i].key);
				break;

			case ArgID_VERSION:
				printf("key: %.*s\n", data[i].key_len, data[i].key);
				break;

			case ArgID_FILE:
				printf("key: %.*s value: %s\n", data[i].key_len, data[i].key, data[i].value);
				break;
		}
	}

	return 0;
}

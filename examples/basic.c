/*
INPUT:
./binary  --file=rom.bin --width=160 --height=144 --fullscreen=true --scale=2.0

OUTPUT:
key: file       value: rom.bin
key: width      value: 160
key: height     value: 144
key: fullscreen value: 1
key: scale      value: 2.0
*/


#include "../args.h"
#include <stdio.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

enum ArgID {
  ArgID_HELP,
  ArgID_VERSION,
  ArgID_FILE,
  ArgID_FULLSCREEN,
  ArgID_WIDTH,
  ArgID_HEIGHT,
  ArgID_SCALE,
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
  		.type = ArgsValueType_NONE,
  		.single = 'h'
  	},
  	{
  		.key = "version",
  		.id = ArgID_VERSION,
  		.type = ArgsValueType_NONE,
  		.single = 'v'
  	},
  	{
  		.key = "file",
  		.id = ArgID_FILE,
  		.type = ArgsValueType_STR,
  		.single = 'f'
  	},
  	{
  		.key = "fullscreen",
  		.id = ArgID_FULLSCREEN,
  		.type = ArgsValueType_BOOL,
  		.single = 0
  	},
  	{
  		.key = "width",
  		.id = ArgID_WIDTH,
  		.type = ArgsValueType_INT,
  		.single = 0
  	},
  	{
  		.key = "height",
  		.id = ArgID_HEIGHT,
  		.type = ArgsValueType_INT,
  		.single = 0
  	},
  	{
  		.key = "scale",
  		.id = ArgID_SCALE,
  		.type = ArgsValueType_DOUBLE,
  		.single = 0
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
  	case ArgsResult_UNWANTED_VALUE:
    case ArgsResult_BAD_VALUE:
    case ArgsResult_MISSING_VALUE:
  	case ArgsResult_ERROR:
  		printf("Args Error: %d\n", result);
  		return -1;

  	case ArgsResult_OK:
  	case ArgsResult_EXTRA_ARGS:
  		break;
  }

  for (unsigned i = 0; i < data_count; ++i) {
    switch (data[i].id) {
    	case ArgID_HELP:
    		printf("key: %.*sn", data[i].key_len, data[i].key);
    		break;

    	case ArgID_VERSION:
    		printf("key: %.*sn", data[i].key_len, data[i].key);
    		break;

    	case ArgID_FILE:
    		printf("key: %.*s\tvalue: %s\n", data[i].key_len, data[i].key, data[i].value.s);
    		break;

    	case ArgID_FULLSCREEN:
    		printf("key: %.*s\tvalue: %d\n", data[i].key_len, data[i].key, data[i].value.b);
    		break;

    	case ArgID_WIDTH:
    		printf("key: %.*s\tvalue: %lld\n", data[i].key_len, data[i].key, data[i].value.i);
    		break;

    	case ArgID_HEIGHT:
    		printf("key: %.*s\tvalue: %lld\n", data[i].key_len, data[i].key, data[i].value.i);
    		break;

    	case ArgID_SCALE:
    		printf("key: %.*s\tvalue: %.1f\n", data[i].key_len, data[i].key, data[i].value.d);
    		break;
    }
  }

  return 0;
}

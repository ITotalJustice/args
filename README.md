# args

Simple c99 argv parser with zero memory allocation

## How to use

args is very simple, it allocates no memory and is only a single function call!

First, create the meta info for the args you want.

```c
enum ArgID {
  ArgID_HELP,
  ArgID_VERSION,
  ArgID_FILE,
};

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
};
```

Next, we need to setup our output data.

NOTE: The number of entries in this array MUST be the same size as the `ArgsMeta` array we created above!

```c
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

struct Argsdata data[ARRAY_SIZE(metas)] = {0};
// this is how many entires are filled by the parser!
unsigned data_count = {0};
```

Finally, we can call the parse function!

```c
const enum ArgsResult result = args_parse(
  1, argc, argv,
  metas, ARRAY_SIZE(metas),
  data, &data_count
);
```

NOTE: You might notice that we pass a `1` as the first param. That's just the index that we want to start from.

Usually, `argv[0]` is the path of the program, so we can skip then, hence passing `1`, so we start from `argv[1]`

The return value of the parser is an enum. in general, `0 == OK`, any negative number is an error, any positive number is `OK` but with with a warning.

```c
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
```

The filled out data struct looks like this:

```c
struct Argsdata {
  const char* key; // key, not always NULL terminated
  unsigned key_len; // actual length of the key
  union ArgsValue value; // union of value types
  int id; // your ID that you set for the key
};
```

where `value` is a union of types:

```c
union ArgsValue {
  const char* s;
  long long i;
  double d;
  bool b;
};
```

You can loop now through the `struct Argsdata` array we created earlier, here's an example

```c
for (unsigned i = 0; i < data_count; ++i) {
  switch (data[i].id) {
    case ArgID_HELP:
      printf("key: %.*s\n", data[i].key_len, data[i].key);
      break;

    case ArgID_VERSION:
      printf("key: %.*s\n", data[i].key_len, data[i].key);
      break;

    case ArgID_FILE:
      printf("key: %.*s value: %s\n", data[i].key_len, data[i].key, data[i].value.s);
      break;
  }
}
```

The full example can be seen [here](https://github.com/ITotalJustice/args/blob/master/examples/basic.c)

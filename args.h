#ifndef _ARGS_H_
#define _ARGS_H_

#ifdef __cplusplus
extern "C" {
#endif


#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_ARGS
    #define ARGS_PUBLIC __declspec(dllexport)
  #else
    #define ARGS_PUBLIC __declspec(dllimport)
  #endif
#else
  #ifdef BUILDING_ARGS
      #define ARGS_PUBLIC __attribute__ ((visibility ("default")))
  #else
      #define ARGS_PUBLIC
  #endif
#endif

enum ArgsMetaType {
	// key only, no value needed
	ArgsMetaType_NO_VALUE,
	// if no value is next, error
	ArgsMetaType_VALUE_REQUIRED,
	// value can be next, or not
	ArgsMetaType_VALUE_OPTIONAL,
};

enum ArgsResult {
	ArgsResult_MISSING_REQUIRED_VALUE = -2,
	ArgsResult_ERROR = -1,
	ArgsResult_OK = 0,
	ArgsResult_EXTRA_ARGS,
};

struct ArgsMeta {
	// the name of the key
	const char* const key;
	// the ID for this key
	int id;
	// see above enum
	enum ArgsMetaType type;
	// enable support for single key (-d) support
	// set to zero to disable this, any value non zero will be tried
	char single;
};

struct Argsdata {
	const char* key;
	// can be NULL if no value, this is always NULL terminated
	const char* value;
	// key is not always NULL terminated if used like key=value
	unsigned key_len;
	// the ID for
	int id;
};

enum ArgsResult ARGS_PUBLIC args_parse(
	// to skip argv[0] (program name), set the index to > 0
	int index, int argc, char* const* const argv,
	// the keys that you want to search
	const struct ArgsMeta* metas, unsigned meta_count,
	// the parsed data out
	struct Argsdata* data_out, unsigned* data_count_out
);

#ifdef __cplusplus
}
#endif

#endif // _ARGS_H_

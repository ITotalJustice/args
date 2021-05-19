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

#include <stdbool.h>


enum ArgsValueType {
  // key only, no value needed
  ArgsValueType_NONE,
  // STR saves ptr to the raw argv value
  ArgsValueType_STR,
  // INT will also handle hex values!
  ArgsValueType_INT,
  // can be used for floats also (of course!)
  ArgsValueType_DOUBLE,
  // BOOL will handle 1,0 OR true,false
  ArgsValueType_BOOL,
};

enum ArgsResult {
  // [ArgsMetaType_NONE] was set but theres a value set
  ArgsResult_UNWANTED_VALUE = -4,
  // got a value missmatch (ie, bool instead of str)
  ArgsResult_BAD_VALUE = -3,
  // value wanted, but didn't get anything
  ArgsResult_MISSING_VALUE = -2,
  // generic error...
  ArgsResult_ERROR = -1,
  // all good!
  ArgsResult_OK = 0,
  // 
  ArgsResult_EXTRA_ARGS = 1,
};

struct ArgsMeta {
  const char* const key;   // the name of the key
  int id;                  // the ID for this key
  enum ArgsValueType type; // value type
  char single;             // if none 0, enables single args, such as -v
};

union ArgsValue {
  const char* s;
  long long i;
  double d;
  bool b;
};

struct Argsdata {
  const char* key;        // key, not always NULL terminated
  unsigned key_len;       // actual length of the key
  union ArgsValue value;  // union of value types
  int id;                 // your ID that you set for the key
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

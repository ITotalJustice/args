#include "args.h"

#include <stddef.h> // for NULL
#include <stdlib.h> // stroll
#include <limits.h> // int limits
#include <float.h> // double limits


struct ArgsIsKeyResult {
	// will be NULL if no key found
	const char* str;
	// length of the key, not NULL terminated
	unsigned len;
	// is the key a single char
	int is_single;
	// is the next char '=', used for seeing is value is part of key str
	int equals_next;
};

// strlen, but for keys (checks for '\0' and '=')
static unsigned keylen(const char* str) {
	unsigned len = 0;

	while (str[len] != '\0' && str[len] != '=') {
		++len;
	}

	return len;
}

// similar to strcmp, but checks that both strings are the same size
// meaning, it'll check up to [stra_len], then check that [strb] ends
// in '\0', meaning we found a match
static bool custom_strncmp(const char* stra, const char* strb, unsigned stra_len) {
	for (unsigned i = 0; i < stra_len; ++i) {
		if (stra[i] != strb[i]) {
			return false;
		}
	}

	if (strb[stra_len] == '\0') {
		return true;
	}

	return false;
}

static struct ArgsIsKeyResult args_is_key(const char* str, int single) {
	struct ArgsIsKeyResult result = {0};
	unsigned offset = 1;

	// check that we have a string and that it starts okay
	if (!str || str[0] != '-') {
		goto fail;
	}

	if (str[1] == '-') {
		if (str[2] == '\0') {
			goto fail;
		}

		offset++;
	}
	
	else if (str[1] == '\0') {
		goto fail;
	}

	else if (!single || (str[1] == '\0' || (str[2] != '\0' && str[2] != '='))) {
		goto fail;
	}

	result.str = str + offset;
	result.len = keylen(result.str);
	result.is_single = offset == 1;
	result.equals_next = result.str[result.len] == '=';

	return result;

fail:
	result.str = NULL;
	return result;
}

static bool get_value_int(const char* str, long long* out) {
	int base = 10;
	int off = 0;

	if (str[0] == '-' || str[0] == '+') {
		off = 1;
	}

	// this allows for support for hex and base 10
	if (str[off] == '0' && (str[off + 1] == 'x' || str[off + 1] == 'X')) {
		base = 16;
	}

	// todo: replace this with my own version, or handle errno.
	// not a fan of errno, so more likely to write my own.
	*out = strtoll(str, NULL, base);

	// potential false positive, would need to check errno
	if (*out == LLONG_MAX || *out == -LLONG_MAX) {
		return false;
	}

	return true;
}

static bool get_value_double(const char* str, double* out) {
	*out = strtod(str, NULL);

	// potential false positive, would need to check errno
	if (*out == DBL_MAX || *out == -DBL_MAX) {
		return false;
	}
	
	return true;
}

static bool get_value_bool(const char* str, bool* out) {
	// booleans can either be "true","false" or "1","0"
	if (custom_strncmp("1", str, 1)) {
		*out = true;
		return true;
	}

	else if (custom_strncmp("0", str, 1)) {
		*out = false;
		return true;
	}

	else if (custom_strncmp("true", str, 4)) {
		*out = true;
		return true;
	}

	else if (custom_strncmp("false", str, 5)) {
		*out = false;
		return true;
	}

	return false;
}

int args_parse(
	// to skip argv[0] (program name), set the index to > 0
	int index, int argc, char* const* const argv,
	// the keys that you want to search
	const struct ArgsMeta* metas, unsigned meta_count,
	// the parsed data out
	struct Argsdata* data_out, unsigned* data_count_out
) {
	// make sure the index is first valid before we use it in arrays!
	if (argc <= 0 || index >= argc) {
		return ArgsResult_ERROR;
	}

	for (int i = index; i < argc; ++i) {
		const struct ArgsIsKeyResult result = args_is_key(argv[i], 1);

		if (result.str == NULL) {
			return ArgsResult_ERROR;
		}

		for (unsigned j = 0; j < meta_count; ++j) {
			if (custom_strncmp(result.str, metas[j].key, result.len) || 
				(result.is_single && metas[j].single && result.str[0] == metas[j].single)
			) {
				// found a key match!
				data_out[*data_count_out].key = result.str;
				data_out[*data_count_out].key_len = result.len;
				data_out[*data_count_out].id = metas[j].id;

				const char* value_str = NULL;

				// if the key was [key=], then the value is part
				// of the same argv[] entry.
				if (result.equals_next) {
					value_str = result.str + result.len + 1;
				}
				// otherwise, the next argv[] is the value (if any)
				else {
					if (i + 1 < argc) {
						const struct ArgsIsKeyResult next_key = args_is_key(argv[i + 1], 1);
						
						// if NULL, next argv is not a key, likely a value
						if (next_key.str == NULL) {
							value_str = argv[i + i];
							++i;
						}
					}
				}

				if (metas[j].type != ArgsValueType_NONE && !value_str) {
					return ArgsResult_MISSING_VALUE;
				}

				switch (metas[j].type) {
					case ArgsValueType_NONE:
						if (value_str) {
							return ArgsResult_UNWANTED_VALUE;
						}
						break;

			  	case ArgsValueType_STR:
			  		data_out[*data_count_out].value.s = value_str;
			  		break;

			  	case ArgsValueType_INT:
			  		if (!get_value_int(
			  			value_str, &data_out[*data_count_out].value.i)
			  		) {
			  			return ArgsResult_BAD_VALUE;
			  		}
			  		break;

			  	case ArgsValueType_DOUBLE:
			  		if (!get_value_double(
			  			value_str, &data_out[*data_count_out].value.d)
			  		) {
			  			return ArgsResult_BAD_VALUE;
			  		}
			  		break;

			  	case ArgsValueType_BOOL:
			  		if (!get_value_bool(
			  			value_str, &data_out[*data_count_out].value.b)
			  		) {
			  			return ArgsResult_BAD_VALUE;
			  		}
			  		break;
				}

				++*data_count_out;

				// make sure we still have space left to handle more keys
				if (*data_count_out >= meta_count) {
					return ArgsResult_EXTRA_ARGS;
				}

				// break out of for loop above as we alread found a key match!
				break;
			}
		}
	}

	return ArgsResult_OK;
}

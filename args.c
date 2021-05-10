#include "args.h"
#include <stddef.h> // for NULL


struct ArgsIsKeyResult {
	// will be NULL if no key found
	const char* str;
	// length of the key, not NULL terminated
	unsigned len;
	// is the key a single char
	int is_single;
	// 
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
static int custom_strncmp(const char* stra, const char* strb, unsigned stra_len) {
	for (unsigned i = 0; i < stra_len; ++i) {
		if (stra[i] != strb[i]) {
			return -1;
		}
	}

	if (strb[stra_len] == '\0') {
		return 0;
	}

	return -1;
}

static struct ArgsIsKeyResult args_is_key(
	const char* str, int single
) {
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
	else if (!single || (str[1] == '\0' || str[2] != '\0')) {
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
			if (!custom_strncmp(result.str, metas[j].key, result.len) || 
				(result.is_single && metas[j].single && result.str[0] == metas[j].single)
			) {
				// found a key match!
				data_out[*data_count_out].key = result.str;
				data_out[*data_count_out].value = NULL;
				data_out[*data_count_out].key_len = result.len;
				data_out[*data_count_out].id = metas[j].id;

				// check if we need data
				switch (metas[j].type) {
					case ArgsMetaType_NO_VALUE:
						break;

					// todo: cleanup the below code
					case ArgsMetaType_VALUE_REQUIRED:
					case ArgsMetaType_VALUE_OPTIONAL: {
						#define BREAK_OR_RETURN(cond) if (cond) { return ArgsResult_MISSING_REQUIRED_VALUE; } else { break; }
						if (result.equals_next) {
							data_out[*data_count_out].value = result.str + result.len + 1;

							if (data_out[*data_count_out].value == NULL) {
								BREAK_OR_RETURN(metas[j].type == ArgsMetaType_VALUE_REQUIRED);
							}
						}
						else {
							if (i + 1 >= argc) {
								BREAK_OR_RETURN(metas[j].type == ArgsMetaType_VALUE_REQUIRED);
							}

							const struct ArgsIsKeyResult v_result = args_is_key(argv[i + 1], 1);

							if (v_result.str) {
								BREAK_OR_RETURN(metas[j].type == ArgsMetaType_VALUE_REQUIRED);
							}

							i++;
							data_out[*data_count_out].value = argv[i];
						}

					}	break;
				}

				++*data_count_out;

				if (*data_count_out >= meta_count) {
					return ArgsResult_EXTRA_ARGS;
				}

				break;
			}
		}
	}

	return ArgsResult_OK;
}

#ifndef YA_GETOPT_H
#define YA_GETOPT_H 1

#if defined(__cplusplus)
extern "C" {
#endif

#define ya_no_argument        0
#define ya_required_argument  1
#define ya_optional_argument  2

    struct option {
        const char* name;
        int has_arg;
        int* flag;
        int val;
    };

    int ya_getopt(int argc, char* const argv[], const char* optstring);
    int ya_getopt_long(int argc, char* const argv[], const char* optstring,
        const struct option* longopts, int* longindex);
    int ya_getopt_long_only(int argc, char* const argv[], const char* optstring,
        const struct option* longopts, int* longindex);

    extern char* ya_optarg;
    extern int ya_optind, ya_opterr, ya_optopt;

#ifndef YA_GETOPT_NO_COMPAT_MACRO
#define getopt ya_getopt
#define getopt_long ya_getopt_long
#define getopt_long_only ya_getopt_long_only
#define optarg ya_optarg
#define optind ya_optind
#define opterr ya_opterr
#define optopt ya_optopt
#define no_argument ya_no_argument
#define required_argument ya_required_argument
#define optional_argument ya_optional_argument
#endif

#if defined(__cplusplus)
}
#endif

#endif
// This file implements some functions as defined in qfits_tools.h
// Implemented using patterns from qfits_tools.c
// and using c++ regex instead of either unix regix or boost regex

#include <regex>

extern "C" {

int qfits_is_int(const char *s) {
    static const std::regex regex_int("^[+-]?[0-9]+$");
    return std::regex_match(s, regex_int) ? 1 : 0;
}

int qfits_is_float(const char *s) {
    static const std::regex regex_float(
        "^[+-]?([0-9]+[.]?[0-9]*|[.][0-9]+)([eEdD][+-]?[0-9]+)?$"
    );
    return std::regex_match(s, regex_float) ? 1 : 0;
}

int qfits_is_complex(const char *s) {
    static const std::regex regex_cmp(
        "^[+-]?([0-9]+[.]?[0-9]*|[.][0-9]+)([eEdD][+-]?[0-9]+)?[ ]+[+-]?([0-9]+[.]?[0-9]*|[.][0-9]+)([eEdD][+-]?[0-9]+)?$"
    );
    return std::regex_match(s, regex_cmp) ? 1 : 0;
}

}

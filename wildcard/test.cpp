/*
** repository: https://github.com/trumanzhao/misc.git
** trumanzhao, 2017-09-07, trumanzhao@foxmail.com
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wildcard.h"

void do_test(const char* text, const char* pattern, bool expected_match, const char* expected_output)
{
    char* my_text = strdup(text);
    const char* match_res = wildcard_match(text, pattern) == expected_match ? "OK" : "Failed";

    int replace_count = 0;
    while (wildcard_replace(my_text, pattern, replace_count, '*') && replace_count > 0)
    {
    }
    const char* replace_res = strcmp(my_text, expected_output) == 0 ? "OK" : "Failed";
    printf("\"%s\":\"%s\"-->%s|%s\n", text, pattern, match_res, replace_res);
    free(my_text);
}

int main()
{
    do_test("", "", true, "");
    do_test("abc", "", false, "abc");
    do_test("abc", "abc", true, "***");
    do_test("abc", "bc", false, "abc");
    do_test("abc", "*", true, "abc");
    do_test("abc", "?bc", true, "a**");
    do_test("abc", "ab?", true, "**c");
    do_test("abc", "*bc", true, "a**");
    do_test("abc", "ab*", true, "**c");
    do_test("abc", "a*c", true, "*b*");
    do_test("abc", "a*bc", true, "***");
    do_test("abc", "a?bc", false, "abc");
    return 0;
}

/*
** repository: https://github.com/trumanzhao/misc.git
** trumanzhao, 2017-09-07, trumanzhao@foxmail.com
*/

#pragma once

template <typename char_type>
bool wildcard_match(const char_type* text, const char_type* pattern)
{
    if (*pattern == '?')
    {
        return wildcard_match(text + 1, pattern + 1);
    }

    if (*pattern == '*')
    {
        do
        {
            if (wildcard_match(text, pattern + 1))
                return true;
        } while (*text++ != '\0');

        return false;
    }

    return *pattern == *text && (*text == '\0' || wildcard_match(text + 1, pattern + 1));
}

template <typename char_type>
bool wildcard_replace(char_type* text, const char_type* pattern, int& replace_count, char_type replace_char)
{
    if (*pattern == '?')
    {
        return wildcard_replace(text + 1, pattern + 1, replace_count, replace_char);
    }

    if (*pattern == '*')
    {
        do
        {
            if (wildcard_replace(text, pattern + 1, replace_count, replace_char))
                return true;
        } while (*text++ != '\0');

        return false;
    }

    if (*pattern == *text)
    {
        if (*text == '\0')
        {
            replace_count = 0;
            return true;
        }

        if (wildcard_replace(text + 1, pattern + 1, replace_count, replace_char))
        {
            *text = replace_char;
            ++replace_count;
            return true;
        }
    }

    return false;
}

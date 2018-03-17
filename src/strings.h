#pragma once

#ifdef _WIN32
    #define stricmp _stricmp
#else 
    #define stricmp strcasecmp
#endif

inline int
StringLength(const char *str)
{
    int result = 0;

    while (*str++)
    {
        ++result;
    }

    return result;
}

inline void
StripCharFromString(char *str, char c)
{
    char *read  = str;
    char *write = str;

    while (*read)
    {
        *write = *read++;

        write += (*write != c);
    }

    *write = '\0';
}

inline int
StringLengthTo(char *str, char stop)
{
	int result = 0;

	while ((*str) && (*str++ != stop))
	{
		++result;
	}

	return result;
}

inline bool
StringContains(char *str, char c)
{
    bool contains = false;

    while (*str && !contains)
    {
        if (*str == c)
        {
            contains = true;
        }

        ++str;
    }

    return contains;
}

template <typename T>
T
CharsToNum(char *str, int base)
{
    T result = {};

    result = static_cast<T>(strtol(str, 0, base));

    return result;
}

inline void
RemoveStartingSpaces(char **str)
{
    while ((**str == ' ') || (**str == '\t'))
    {
        ++*str;
    }
}


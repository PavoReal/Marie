#pragma once

#ifdef _WIN32
    #define stricmp _stricmp
#else 
    #define stricmp strcasecmp
#endif

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

inline size_t
StringLengthTo(char *str, char stop)
{
	size_t result = 0;

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
DecCharsToNum(char *str, size_t length)
{
    T result = {};

	while (length--)
	{
		result = result * 10 + (*str++ - '0');
	}

    return result;
}

template <typename T>
T
HexCharsToNum(char *str)
{
    T result = {};

    result = static_cast<T>(strtol(str, 0, 16));

    return result;
}

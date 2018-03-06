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


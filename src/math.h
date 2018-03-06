#pragma once

template <typename T>
T
CharsToNum(char *str, size_t length)
{
    T result = {};

	while (length--)
	{
		result = result * 10 + (*str++ - '0');
	}

    return result;
}


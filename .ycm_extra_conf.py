def FlagsForFile( filename, **kwards ):
    return {
            'flags' : [ '-x', 'c++', '-Wall', '-Wextra', '-std=c++14', '-g', '-O0', '-fno-exceptions', '-fno-rtti' ]
    }


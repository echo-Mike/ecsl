# echo-mike's snippets library (ECSL)

This is a collection of header-only C++ snippets used in various projects.  

## Caveats

This snippets are made using the C++11 and higher versions of language in mind and have little to no concerns about C++98 restrictions.  
Everything is put into namespace `ecsl` and sub-namespaces are created where necessary.  
If You found Yourself using something with qualified name containing `detail` then You probably doing something wrong.  
There is an entire macro library present that uses macro names that starts with `$` symbol. Your compiler may not supports them.  
All macro that are defined by this library is either:  

1. starts with `$` or
2. starts with `ECSL_` and ends on letter or number, or
3. starts with `ECSL_` and ends with `_`

The macro of the first and second type are intended to be used by an user of the library and macro of the third type are internal ones

## Content

| Folder | Description |
|--------|-------------|

## Building

No build requited just include what You use

## Licensing

See file LICENSE

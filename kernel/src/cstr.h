#pragma once
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

size_t strlen(const char*);
int strncmp(const char *s1, const char *s2, int c);
char *strstr(const char *in, const char *str);
char *strsep(char **stringp, const char *delim);
char *strdup(const char *src);
int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
void* memset64(void*, uint64_t, size_t);
int strint(const char *str);
void strcpy(char *str1, char *str2);
unsigned int is_delim(char c, char *delim);
char *strtok(char *srcString, char *delim);
uint8_t *strsplit(uint8_t *str, uint8_t delim);
char *strlower(char *str);
char *strupper(char *str);
int strcmp(const char *str1, const char *str2);
void strshf(char *str, int start, int end, int shift);
char *strcat(char*, const char*);
int strfind(char* string, char c);

const char* to_string(uint64_t value);
const char* to_string(int64_t value);
const char* to_hstring(uint64_t value);
const char* to_hstring(uint32_t value);
const char* to_hstring(uint16_t value);
const char* to_hstring(uint8_t value);
const char* to_string(double value, uint8_t decimalPlaces);
const char* to_string(double value);

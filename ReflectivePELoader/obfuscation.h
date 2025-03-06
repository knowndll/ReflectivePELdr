#pragma once
#ifndef OBFUSCATION_H
#define OBFUSCATION_H

#include <stdlib.h>
#include <Windows.h>
#include <string.h>
#include <stdio.h>

char* base64(const void* data, const size_t len, size_t* decoded_size);
void xor(char* input, size_t sDataSize, const char* key);

extern const int B64index[256];

#endif
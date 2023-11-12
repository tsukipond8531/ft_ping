#ifndef UTILS_H_
#define UTILS_H_
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint16_t compute_checksum(void const *bytes, size_t const number);
bool is_valid_checksum(void const *bytes, size_t const number);

#endif // !UTILS_H_

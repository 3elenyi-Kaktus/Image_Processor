#ifndef PROGRAM_IMAGE_PARSERS_H
#define PROGRAM_IMAGE_PARSERS_H


#include "image.h"
#include "logger.h"


void set8bit(uint16_t data, char buffer[]);
void set16bit(uint16_t data, char buffer[]);
void set32bit(uint32_t data, char buffer[]);

constexpr int64_t get8bit(const char buffer[]);
constexpr int64_t get16bit(const char buffer[]);
constexpr int64_t get32bit(const char buffer[]);

Image PrepareInputImage(const std::string &path);
void GetOutputImage(Image &image, const std::string &path);


#endif //PROGRAM_IMAGE_PARSERS_H

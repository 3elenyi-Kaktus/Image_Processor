#ifndef PROGRAM_IMAGE_H
#define PROGRAM_IMAGE_H

#include <cstdint>
#include <deque>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <string>


struct UnlimitedColours {
    int64_t r, g, b;
};

struct Colours {
    uint8_t r, g, b;
};

using Data = std::deque<std::vector<Colours>>;
using ASCII_Data = std::unordered_map<int, Data>;

class BMPInfo {
public:
    BMPInfo() : file_size(0), reserved_field(0), file_offset(54), DIB_header_size(40), width(0), height(0), planes(1),
                bits_per_pixel(24), compression(0), image_size(0), x_pixels(4000), y_pixels(4000), colours(0),
                important_colours(0) {}

    int64_t file_size;
    int64_t reserved_field;
    int64_t file_offset;
    int64_t DIB_header_size;
    int64_t width;
    int64_t height;
    int64_t planes;
    int64_t bits_per_pixel;
    int64_t compression;
    int64_t image_size;
    int64_t x_pixels;
    int64_t y_pixels;
    int64_t colours;
    int64_t important_colours;
};

class Image {
public:
    Image() : data_(), header_() {}

    Data data_;
    BMPInfo header_;
};


#endif //PROGRAM_IMAGE_H

#include "image_parsers.h"


void set8bit(uint16_t data, char buffer[]) {
    buffer[0] = static_cast<char>(data);
}

void set16bit(uint16_t data, char buffer[]) {
    buffer[0] = static_cast<char>(data);
    buffer[1] = static_cast<char>(data >> 8);
}

void set32bit(uint32_t data, char buffer[]) {
    buffer[0] = static_cast<char>(data);
    buffer[1] = static_cast<char>(data >> 8);
    buffer[2] = static_cast<char>(data >> 16);
    buffer[3] = static_cast<char>(data >> 24);
}


constexpr int64_t get8bit(const char buffer[]) {
    return static_cast<int64_t>(static_cast<uint8_t>(buffer[0]));
}

constexpr int64_t get16bit(const char buffer[]) {
    return static_cast<int64_t>(static_cast<uint8_t>(buffer[0])) |
           static_cast<int64_t>(static_cast<uint8_t>(buffer[1])) << 8;
}

constexpr int64_t get32bit(const char buffer[]) {
    return static_cast<int64_t>(static_cast<uint8_t>(buffer[0])) |
           static_cast<int64_t>(static_cast<uint8_t>(buffer[1])) << 8 |
           static_cast<int64_t>(static_cast<uint8_t>(buffer[2])) << 16 |
           static_cast<int64_t>(static_cast<uint8_t>(buffer[3])) << 24;
}


Image PrepareInputImage(const std::string &path) {
    Logger logger;
    char buffer[54];
    std::ifstream file(path, std::ios::binary);


    try {
        if (!file.is_open()) {
            throw std::runtime_error("Incorrect input file path!\n");
        }
    } catch (std::runtime_error &error) {
        logger.AddLog(error.what());
        FatalError();
    }

    Image image;
    logger.AddLog("Preparation of the file " + path + "\n");
    try {
        file.read(buffer, 54);
        if (buffer[0] != 'B' || buffer[1] != 'M') {
            throw std::runtime_error("This file is not a BMP!\n");
        }
    } catch (std::runtime_error &error) {
        logger.AddLog(error.what());
        FatalError();
    }
    logger.AddLog("signature: BM");

    image.header_.file_size = get32bit(&buffer[2]);
    logger.AddLog("file_size " + std::to_string(image.header_.file_size));

    image.header_.reserved_field = get32bit(&buffer[6]);
    logger.AddLog("reserved field " + std::to_string(image.header_.reserved_field));

    image.header_.file_offset = get32bit(&buffer[10]);
    logger.AddLog("offset to pixels " + std::to_string(image.header_.file_offset));

    image.header_.DIB_header_size = get32bit(&buffer[14]);
    logger.AddLog("DIB_header_size " + std::to_string(image.header_.DIB_header_size));

    image.header_.width = get32bit(&buffer[18]);
    logger.AddLog("width " + std::to_string(image.header_.width));

    image.header_.height = get32bit(&buffer[22]);
    logger.AddLog("height " + std::to_string(image.header_.height));

    image.header_.planes = get16bit(&buffer[26]);
    logger.AddLog("planes " + std::to_string(image.header_.planes));

    image.header_.bits_per_pixel = get16bit(&buffer[28]);
    logger.AddLog("bits_per_pixel " + std::to_string(image.header_.bits_per_pixel));

    image.header_.compression = get32bit(&buffer[30]);
    logger.AddLog("compression " + std::to_string(image.header_.compression));

    image.header_.image_size = get32bit(&buffer[34]);
    logger.AddLog("image_size " + std::to_string(image.header_.image_size));

    image.header_.x_pixels = get32bit(&buffer[38]);
    logger.AddLog("x_pixels " + std::to_string(image.header_.x_pixels));

    image.header_.y_pixels = get32bit(&buffer[42]);
    logger.AddLog("y_pixels " + std::to_string(image.header_.y_pixels));

    image.header_.colours = get32bit(&buffer[46]);
    logger.AddLog("colours " + std::to_string(image.header_.colours));

    image.header_.important_colours = get32bit(&buffer[50]);
    logger.AddLog("important colours " + std::to_string(image.header_.important_colours) + "\n");
    logger.AddLog("Finished header reading.");

    try {
        if (image.header_.file_offset != 54 ||
            image.header_.DIB_header_size != 40 ||
            image.header_.planes != 1 ||
            image.header_.bits_per_pixel != 24 ||
            image.header_.compression != 0 ||
            image.header_.colours != 0 ||
            image.header_.important_colours != 0) {
            throw std::runtime_error("Input file is damaged! Incorrect BMP header file behaviour.\n");
        }
    } catch (std::runtime_error &error) {
        logger.AddLog(error.what());
        FatalError();
    }

    char pixel_buffer[image.header_.width * 3];
    int64_t current_offset = image.header_.file_offset;
    int64_t current_inline_offset;
    int64_t additional_padding = (4 - (image.header_.width * 3) % 4) % 4;
    file.seekg(current_offset);
    logger.AddLog("Starting preparation of the pixel array.");
    Data data(image.header_.height);
    for (size_t i = 0; i < image.header_.height; ++i) {
        data[i].resize(image.header_.width);
        file.read(pixel_buffer, image.header_.width * 3);
        current_inline_offset = 0;
        for (size_t j = 0; j < image.header_.width; ++j) {
            data[i][j].b = get8bit(&pixel_buffer[current_inline_offset]);
            data[i][j].g = get8bit(&pixel_buffer[current_inline_offset + 1]);
            data[i][j].r = get8bit(&pixel_buffer[current_inline_offset + 2]);
            current_inline_offset += 3;
        }
        current_offset += image.header_.width * 3 + additional_padding;
        file.seekg(current_offset);
    }
    file.close();
    image.data_ = data;
    logger.AddLog("Finished pixel array reading.\n\n\n");
    return image;
}


void GetOutputImage(Image &image, const std::string &path) {
    Logger logger;
    std::ofstream outfile(path, std::ios::binary);
    char buffer[54];
    int64_t additional_padding = (4 - (3 * image.header_.width) % 4) % 4;
    image.header_.file_size = (image.header_.width * 3 + additional_padding) * image.header_.height + 54;
    image.header_.image_size = (image.header_.width * 3 + additional_padding) * image.header_.height;

    logger.AddLog("Starting writing file to the " + path);
    buffer[0] = 'B', buffer[1] = 'M';
    logger.AddLog("signature: BM");

    set32bit(image.header_.file_size, &buffer[2]);
    logger.AddLog("file_size " + std::to_string(image.header_.file_size));

    set32bit(image.header_.reserved_field, &buffer[6]);
    logger.AddLog("reserved field " + std::to_string(image.header_.reserved_field));

    set32bit(image.header_.file_offset, &buffer[10]);
    logger.AddLog("offset to pixels " + std::to_string(image.header_.file_offset));

    set32bit(image.header_.DIB_header_size, &buffer[14]);
    logger.AddLog("DIB_header_size " + std::to_string(image.header_.DIB_header_size));

    set32bit(image.header_.width, &buffer[18]);
    logger.AddLog("width " + std::to_string(image.header_.width));

    set32bit(image.header_.height, &buffer[22]);
    logger.AddLog("height " + std::to_string(image.header_.height));

    set16bit(image.header_.planes, &buffer[26]);
    logger.AddLog("planes " + std::to_string(image.header_.planes));

    set16bit(image.header_.bits_per_pixel, &buffer[28]);
    logger.AddLog("bits_per_pixel " + std::to_string(image.header_.bits_per_pixel));

    set32bit(image.header_.compression, &buffer[30]);
    logger.AddLog("compression " + std::to_string(image.header_.compression));

    set32bit(image.header_.image_size, &buffer[34]);
    logger.AddLog("image_size " + std::to_string(image.header_.image_size));

    set32bit(image.header_.x_pixels, &buffer[38]);
    logger.AddLog("x_pixels " + std::to_string(image.header_.x_pixels));

    set32bit(image.header_.y_pixels, &buffer[42]);
    logger.AddLog("y_pixels " + std::to_string(image.header_.y_pixels));

    set32bit(image.header_.colours, &buffer[46]);
    logger.AddLog("colours " + std::to_string(image.header_.colours));

    set32bit(image.header_.important_colours, &buffer[50]);
    logger.AddLog("important colours " + std::to_string(image.header_.important_colours) + "\n");
    outfile.write(buffer, 54);
    logger.AddLog("Finished header writing.");


    char pixel_buffer[image.header_.width * 3];
    int64_t current_offset = image.header_.file_offset;
    int64_t current_inline_offset;
    outfile.seekp(current_offset);


    logger.AddLog("Starting writing of the pixel array.");
    for (const auto &row: image.data_) {
        current_inline_offset = 0;
        for (auto ind: row) {
            set8bit(ind.b, &pixel_buffer[current_inline_offset]);
            set8bit(ind.g, &pixel_buffer[current_inline_offset + 1]);
            set8bit(ind.r, &pixel_buffer[current_inline_offset + 2]);
            current_inline_offset += 3;
        }
        outfile.write(pixel_buffer, image.header_.width * 3);
        current_offset += image.header_.width * 3 + additional_padding;
        outfile.seekp(current_offset);
    }
    outfile.write(pixel_buffer, additional_padding);
    outfile.close();
    logger.AddLog("Finished pixel array writing.\n\n\n");
}

#include "filters.h"

Image ProtoFilter::ApplyFilter(Image &image) {
    image_ = image;
    return image_;
}

Image Crop::ApplyFilter(Image &image) {
    image_ = image;
    if (height_ < image_.header_.height) {
        for (size_t i = 0; i < image_.header_.height - height_; ++i) {
            image_.data_.pop_front();
        }
        image_.header_.height = height_;
    }
    if (width_ < image_.header_.width) {
        for (auto &row: image_.data_) {
            row.resize(width_);
        }
        image_.header_.width = width_;
    }
    image_.header_.file_size = image_.header_.height * image_.header_.width * 3 + image_.header_.file_offset;
    image_.header_.image_size = image_.header_.height * image_.header_.width * 3;
    return image_;
}

Image Negative::ApplyFilter(Image &image) {
    image_ = image;
    for (auto &row: image_.data_) {
        for (auto &pixel: row) {
            pixel.b = 255 - pixel.b;
            pixel.g = 255 - pixel.g;
            pixel.r = 255 - pixel.r;
        }
    }
    return image_;
}

Image Greyscale::ApplyFilter(Image &image) {
    image_ = image;
    for (auto &row: image_.data_) {
        for (auto &pixel: row) {
            pixel.b = pixel.g = pixel.r = round(0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b);
        }
    }
    return image_;
}

Image Sharpening::ApplyFilter(Image &image) {
    image_ = image;
    Data out_data(image_.data_.size());
    for (auto &row: out_data) {
        row.resize(image_.data_[0].size());
    }
    std::vector<int> matrix{0, -1, 0, -1, 5, -1, 0, -1, 0};
    UnlimitedColours pixel{.r = 0, .g = 0, .b = 0};
    int matrix_count;
    size_t a, b;
    for (int64_t i = 0; i < image_.data_.size(); ++i) {
        for (int64_t j = 0; j < image_.data_[0].size(); ++j) {
            pixel.r = pixel.g = pixel.b = 0;
            matrix_count = 0;
            for (int64_t x = i - 1; x < i + 2; ++x) {
                for (int64_t y = j - 1; y < j + 2; ++y) {
                    a = std::clamp(x, static_cast<int64_t>(0), static_cast<int64_t>(image_.data_.size() - 1));
                    b = std::clamp(y, static_cast<int64_t>(0), static_cast<int64_t>(image_.data_[0].size() - 1));
                    pixel.r += image_.data_[a][b].r * matrix[matrix_count];
                    pixel.g += image_.data_[a][b].g * matrix[matrix_count];
                    pixel.b += image_.data_[a][b].b * matrix[matrix_count];
                    ++matrix_count;
                }
            }
            pixel.r = std::clamp(pixel.r, static_cast<int64_t>(0), static_cast<int64_t>(255));
            pixel.g = std::clamp(pixel.g, static_cast<int64_t>(0), static_cast<int64_t>(255));
            pixel.b = std::clamp(pixel.b, static_cast<int64_t>(0), static_cast<int64_t>(255));
            out_data[i][j].r = pixel.r;
            out_data[i][j].g = pixel.g;
            out_data[i][j].b = pixel.b;
        }
    }
    image_.data_ = out_data;
    return image_;
}

Image EdgeDetection::ApplyFilter(Image &image) {
    image_ = image;
    Data out_data(image_.data_.size());
    for (auto &row: out_data) {
        row.resize(image_.data_[0].size());
    }
    std::vector<int> matrix{0, -1, 0, -1, 4, -1, 0, -1, 0};
    UnlimitedColours pixel{};
    int matrix_count;
    size_t a, b;
    for (int64_t i = 0; i < image_.data_.size(); ++i) {
        for (int64_t j = 0; j < image_.data_[0].size(); ++j) {
            pixel.r = 0;
            matrix_count = 0;
            for (int64_t x = i - 1; x < i + 2; ++x) {
                for (int64_t y = j - 1; y < j + 2; ++y) {
                    a = std::clamp(x, static_cast<int64_t>(0), static_cast<int64_t>(image_.data_.size() - 1));
                    b = std::clamp(y, static_cast<int64_t>(0), static_cast<int64_t>(image_.data_[0].size() - 1));
                    pixel.r += image_.data_[a][b].r * matrix[matrix_count];
                    ++matrix_count;
                }
            }
            if (pixel.r >= threshold_) {
                out_data[i][j].r = out_data[i][j].g = out_data[i][j].b = 255;
            } else {
                out_data[i][j].r = out_data[i][j].g = out_data[i][j].b = 0;
            }
        }
    }
    image_.data_ = out_data;
    return image_;
}

Image GaussianBlur::ApplyFilter(Image &image) {
    image_ = image;
    Logger logger;
    std::vector<double> matrix;
    double coefficient;
    for (int64_t i = 3 * sigma_; i >= 0; --i) {
        coefficient = 1 / (sqrt(2 * std::numbers::pi * sigma_ * sigma_) * exp((i * i) / (2 * sigma_ * sigma_)));
        matrix.push_back(coefficient);
    }
    for (int64_t i = matrix.size() - 2; i >= 0; --i) {
        matrix.push_back(matrix[i]);
    }


    std::string coeff;
    for (auto elem: matrix) {
        coeff += std::to_string(elem) + " ";
    }
    logger.AddLog("Counted coefficients based on sigma input:\n" + coeff);


    double correction_coefficient = 0;
    for (auto elem: matrix) {
        correction_coefficient += elem;
    }
    logger.AddLog("Current sum of coefficients in matrix: " + std::to_string(correction_coefficient));
    for (auto &elem: matrix) {
        elem = elem / correction_coefficient;
    }


    Data out_data(image_.data_.size());
    for (auto &row: out_data) {
        row.resize(image_.data_[0].size());
    }
    UnlimitedColours pixel{};
    int64_t position;
    for (int64_t i = 0; i < out_data.size(); ++i) {
        for (int64_t j = 0; j < out_data[0].size(); ++j) {
            pixel.r = pixel.g = pixel.b = 0;
            for (int64_t k = 0; k < matrix.size(); ++k) {
                position = std::clamp(j - (static_cast<int64_t>(matrix.size()) - 1) / 2 + k, static_cast<int64_t>(0),
                                      static_cast<int64_t>(image_.data_[0].size()) - 1);
                pixel.r += round(image_.data_[i][position].r * matrix[k]);
                pixel.g += round(image_.data_[i][position].g * matrix[k]);
                pixel.b += round(image_.data_[i][position].b * matrix[k]);
            }
            out_data[i][j].r = std::clamp(pixel.r, static_cast<int64_t>(0), static_cast<int64_t>(255));
            out_data[i][j].g = std::clamp(pixel.g, static_cast<int64_t>(0), static_cast<int64_t>(255));
            out_data[i][j].b = std::clamp(pixel.b, static_cast<int64_t>(0), static_cast<int64_t>(255));
        }
    }
    image_.data_ = out_data;
    for (int64_t i = 0; i < out_data[0].size(); ++i) {
        for (int64_t j = 0; j < out_data.size(); ++j) {
            pixel.r = pixel.g = pixel.b = 0;
            for (int64_t k = 0; k < matrix.size(); ++k) {
                position = std::clamp(j - (static_cast<int64_t>(matrix.size()) - 1) / 2 + k, static_cast<int64_t>(0),
                                      static_cast<int64_t>(image_.data_.size()) - 1);
                pixel.r += round(image_.data_[position][i].r * matrix[k]);
                pixel.g += round(image_.data_[position][i].g * matrix[k]);
                pixel.b += round(image_.data_[position][i].b * matrix[k]);
            }
            out_data[j][i].r = std::clamp(pixel.r, static_cast<int64_t>(0), static_cast<int64_t>(255));
            out_data[j][i].g = std::clamp(pixel.g, static_cast<int64_t>(0), static_cast<int64_t>(255));
            out_data[j][i].b = std::clamp(pixel.b, static_cast<int64_t>(0), static_cast<int64_t>(255));
        }
    }
    image_.data_ = out_data;
    return image_;
}

int ASCII::GetPixelsBrightness(int64_t start_height, int64_t start_width) {
    double total_brightness = 0;
    int average_brightness;
    int64_t real_height = start_height * 15;
    int64_t real_width = start_width * 9;
    for (size_t i = real_height; i < real_height + 15; ++i) {
        for (size_t j = real_width; j < real_width + 9; ++j) {
            total_brightness +=
                    image_.data_[i][j].r * 0.212 + image_.data_[i][j].g * 0.715 + image_.data_[i][j].b * 0.072;
        }
    }
    average_brightness = floor(total_brightness / 135 / 255 * 100);
    return average_brightness;
}

Image ASCII::ApplyFilter(Image &image) {
    image_ = image;
    int light_level = -1;
    std::vector<std::string> characters = {"empty", "dot", "double_dot", "r", "4", "w", "dog"};
    for (auto &file_name: characters) {
        std::string path = "./Symbols/" + file_name + ".bmp";
        Image ascii_image = PrepareInputImage(path);
        light_level += 1;
        ASCII_data_[light_level] = ascii_image.data_;
    }


    std::vector<std::vector<int>> brightness_table;
    std::vector<int> one_line;
    int area_brightness;
    int br_level;
    int64_t height_in_ASCII = image_.data_.size() / 15;
    int64_t width_in_ASCII = image_.data_[0].size() / 9;
    int64_t current_pos_height = 0;
    int64_t current_pos_width;
    while (current_pos_height < height_in_ASCII) {
        current_pos_width = 0;
        one_line.clear();
        while (current_pos_width < width_in_ASCII) {
            area_brightness = GetPixelsBrightness(current_pos_height, current_pos_width);
            br_level = round(area_brightness / 14.);
            one_line.push_back(br_level);
            ++current_pos_width;
        }
        brightness_table.push_back(one_line);
        ++current_pos_height;
    }


    int brightness;
    image_.data_.resize(height_in_ASCII * 15);
    for (auto &row: image_.data_) {
        row.resize(width_in_ASCII * 9);
    }
    for (size_t row = 0; row < height_in_ASCII; ++row) {
        for (size_t col = 0; col < width_in_ASCII; ++col) {
            brightness = std::clamp(brightness_table[row][col], 0, 6);
            for (int i = 0; i < 15; ++i) {
                for (int j = 0; j < 9; ++j) {
                    image_.data_[row * 15 + i][col * 9 + j] = ASCII_data_[brightness][i][j];
                }
            }
        }
    }
    image_.header_.file_size = image_.data_.size() * image_.data_[0].size() * 3 + 54;
    image_.header_.width = image_.data_[0].size();
    image_.header_.height = image_.data_.size();
    image_.header_.image_size = image_.data_.size() * image_.data_[0].size() * 3;
    return image_;
}

Image Random::ApplyFilter(Image &image) {
    image_ = image;
    Data out_data(height_);
    for (auto &row: out_data) {
        row.resize(width_);
    }
    image_.data_ = out_data;
    image_.header_.file_size = width_ * height_ * 3 + 54;
    image_.header_.width = width_;
    image_.header_.height = height_;
    image_.header_.image_size = width_ * height_ * 3;

    for (size_t i = 0; i < image_.header_.height; ++i) {
        for (size_t j = 0; j < image_.header_.width; ++j) {
            image_.data_[i][j].r = rand() % 256;
            image_.data_[i][j].g = rand() % 256;
            image_.data_[i][j].b = rand() % 256;
        }
    }
    return image_;
}



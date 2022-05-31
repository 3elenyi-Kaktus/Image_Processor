#ifndef PROGRAM_FILTERS_H
#define PROGRAM_FILTERS_H


#include "image.h"
#include "image_parsers.h"


class ProtoFilter {
public:
    ProtoFilter() : image_() {};

    explicit ProtoFilter(Image &image) : image_(image) {}

    virtual Image ApplyFilter(Image &image);

    Image image_;
};


class Crop : virtual public ProtoFilter {
public:
    Crop(int64_t width, int64_t height) : ProtoFilter(), width_(width), height_(height) {}

    Image ApplyFilter(Image &image) override;

private:
    int64_t width_;
    int64_t height_;
};


class Negative : virtual public ProtoFilter {
public:
    explicit Negative() : ProtoFilter() {}

    Image ApplyFilter(Image &image) override;
};


class Greyscale : virtual public ProtoFilter {
public:
    explicit Greyscale() : ProtoFilter() {}

    Image ApplyFilter(Image &image) override;
};

class Sharpening : virtual public ProtoFilter {
public:
    explicit Sharpening() : ProtoFilter() {}

    Image ApplyFilter(Image &image) override;
};

class EdgeDetection : virtual public ProtoFilter {
public:
    explicit EdgeDetection(double threshold) : ProtoFilter(), threshold_(round(threshold * 255)) {}

    Image ApplyFilter(Image &image) override;

private:
    int64_t threshold_;
};

class GaussianBlur : virtual public ProtoFilter {
public:
    explicit GaussianBlur(double sigma) : ProtoFilter(), sigma_(sigma) {}

    Image ApplyFilter(Image &image) override;

private:
    double sigma_;
};


class ASCII : virtual public ProtoFilter {
public:
    explicit ASCII() : ProtoFilter() {}

    Image ApplyFilter(Image &image) override;

    int GetPixelsBrightness(int64_t start_height, int64_t start_width);

private:
    ASCII_Data ASCII_data_;
};

class Random : virtual public ProtoFilter {
public:
    Random(int64_t height, int64_t width) : ProtoFilter(), width_(width), height_(height) {}

    Image ApplyFilter(Image &image) override;

private:
    int64_t width_;
    int64_t height_;
};


#endif //PROGRAM_FILTERS_H

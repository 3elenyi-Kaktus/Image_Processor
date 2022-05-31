#ifndef PROGRAM_FILTERS_APPLIER_H
#define PROGRAM_FILTERS_APPLIER_H


#include "filters.h"
#include "string_parser.h"
#include "logger.h"
#include "image_parsers.h"


using Pipeline = std::vector<std::shared_ptr<ProtoFilter>>;


std::shared_ptr<Crop> MakeCrop(OneFilterParameters params);
std::shared_ptr<Negative> MakeNegative(const OneFilterParameters &params);
std::shared_ptr<Greyscale> MakeGreyscale(const OneFilterParameters &params);
std::shared_ptr<Sharpening> MakeSharpening(const OneFilterParameters &params);
std::shared_ptr<EdgeDetection> MakeEdgesDetection(OneFilterParameters params);
std::shared_ptr<GaussianBlur> MakeGaussianBlur(OneFilterParameters params);
std::shared_ptr<ASCII> MakeASCII(const OneFilterParameters &params);
std::shared_ptr<Random> MakeRandom(OneFilterParameters params);

Image ApplyFilters(const Pipeline &pipeline, Image &image);
void Execute(int argc, char *argv[]);


#endif //PROGRAM_FILTERS_APPLIER_H

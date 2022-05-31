#include "filters_applier.h"


std::shared_ptr<Crop> MakeCrop(OneFilterParameters params) {
    return std::make_shared<Crop>(std::stoi(params["width"]), std::stoi(params["height"]));
}

std::shared_ptr<Negative> MakeNegative(const OneFilterParameters &params) {
    return std::make_shared<Negative>();
}

std::shared_ptr<Greyscale> MakeGreyscale(const OneFilterParameters &params) {
    return std::make_shared<Greyscale>();
}

std::shared_ptr<Sharpening> MakeSharpening(const OneFilterParameters &params) {
    return std::make_shared<Sharpening>();
}

std::shared_ptr<EdgeDetection> MakeEdgesDetection(OneFilterParameters params) {
    return std::make_shared<EdgeDetection>(std::stod(params["threshold"]));
}

std::shared_ptr<GaussianBlur> MakeGaussianBlur(OneFilterParameters params) {
    return std::make_shared<GaussianBlur>(std::stod(params["sigma"]));
}

std::shared_ptr<ASCII> MakeASCII(const OneFilterParameters &params) {
    return std::make_shared<ASCII>();
}

std::shared_ptr<Random> MakeRandom(OneFilterParameters params) {
    return std::make_shared<Random>(std::stoi(params["height"]), std::stoi(params["width"]));
}


Image ApplyFilters(const Pipeline &pipeline, Image &image) {
    Logger logger;
    logger.AddLog("Started filters applying.");
    Image result = image;
    for (const auto &filter: pipeline) {
        result = filter->ApplyFilter(result);
    }
    logger.AddLog("Filters applying finished.");
    return result;
}

Pipeline MakePipeline(const FiltersQueue &filters_queue) {
    Logger logger;
    Pipeline pipeline;
    for (const auto &filter: filters_queue) {
        logger.AddLog("Adding the " + filter.first + " filter to the pipeline.");
        if (filter.first == "-crop") {
            pipeline.push_back(MakeCrop(filter.second));
        } else if (filter.first == "-neg") {
            pipeline.push_back(MakeNegative(filter.second));
        } else if (filter.first == "-gs") {
            pipeline.push_back(MakeGreyscale(filter.second));
        } else if (filter.first == "-sharp") {
            pipeline.push_back(MakeSharpening(filter.second));
        } else if (filter.first == "-edge") {
            pipeline.push_back(MakeGreyscale(filter.second));
            pipeline.push_back(MakeEdgesDetection(filter.second));
        } else if (filter.first == "-ascii") {
            pipeline.push_back(MakeASCII(filter.second));
        } else if (filter.first == "-random") {
            pipeline.push_back(MakeRandom(filter.second));
        } else if (filter.first == "-blur") {
            pipeline.push_back(MakeGaussianBlur(filter.second));
        }
    }
    return pipeline;
}


void Execute(int argc, char *argv[]) {
    Logger logger;
    logger.ClearLog();
    logger.AddLog("Program execution process started.");
    std::cout << "Program execution process started.\n" << std::endl;
    Parser parser(argc, argv);
    parser.ParseCommandLine();
    Image image = PrepareInputImage(parser.GetInputPath());
    FiltersQueue filters_queue = parser.GetFiltersQueue();
    Pipeline pipeline = MakePipeline(filters_queue);
    image = ApplyFilters(pipeline, image);
    GetOutputImage(image, parser.GetOutputPath());
    logger.AddLog("Program execution process finished.");
    std::cout << "Program execution process finished.\n"
                 "See the error_log file if you would like to get more detailed information.\n"
                 "Press \"Enter\" to exit the program." << std::endl;
    getchar();
}

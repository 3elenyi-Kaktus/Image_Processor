#include "string_parser.h"


void Parser::AddFilePaths() {
    Logger logger;
    std::vector<std::string> files = {"input_file", "output_file"};
    int count = 1;
    for (auto &file: files) {
        file_paths_[file] = argv_[count];
        logger.AddLog("Added to " + file + " path: " + argv_[count]);
        ++count;
    }
}


void Parser::AddReferenceFilter(const std::string &keyword, const OneFilterParameters &params) {
    filters_keywords_[keyword] = params;
}


void Parser::ParseCommandLine() {
    Logger logger;
    if (argc_ == 1) {
        std::cout << "Input format:\n"
                     "{program name} {path to input file} {path to output file} [-{filter name 1} [parameter 1] [parameter 2] ...] [-{filter name 2} [parameter 1] [parameter 2] ...] ...\n\n"
                     "Current available functions list:\n\n"
                     "-crop [width][height]\nCrops an image to the [width][height] coordinates (counting from the upper left corner).\n"
                     "-gs\nTransforms any colours to the shadows of grey.\n"
                     "-neg\nCreates a negative image.\n"
                     "-sharp\nSharpens the image.\n"
                     "-edge [threshold]\nCreates an edged image based on [threshold] parameter.\n"
                     "-blur [sigma]\nBlurs the image based on [sigma] parameter.\n"
                     "-ascii\nTransforms an image into image compilation of ASCII symbols.\n"
                     "-random [width][height]\nCreates an image with random pixel colours with size based on [width][height] parameters.\n\n"
                     "Press \"Enter\" to continue." << std::endl;
        logger.AddLog("Program execution process finished.");
        getchar();
        std::cout << "Program execution process finished." << std::endl;
        exit(0);
    }
    try {
        if (1 < argc_ && argc_ < 4) {
            throw std::runtime_error("Incorrect commands input detected.");
        }
    } catch (std::runtime_error &error) {
        logger.AddLog(error.what());
        FatalError();
    }
    AddFilePaths();
    logger.AddLog("Got " + std::to_string(argc_) + " arguments.");


    AddReferenceFilter("-crop", {{"width",  "int"},
                                 {"height", "int"}});
    AddReferenceFilter("-neg", {});
    AddReferenceFilter("-gs", {});
    AddReferenceFilter("-sharp", {});
    AddReferenceFilter("-edge", {{"threshold", "float"}});
    AddReferenceFilter("-blur", {{"sigma", "float"}});
    AddReferenceFilter("-ascii", {});
    AddReferenceFilter("-random", {{"width",  "int"},
                                   {"height", "int"}});


    std::regex integer(R"(\d+)");
    std::regex floating(R"(\d+\.\d+)");
    int64_t pos_in_command_line = 3;
    int64_t argument_pos;
    std::string filter_name;
    while (pos_in_command_line < argc_) {

        try {
            if (!filters_keywords_.contains(argv_[pos_in_command_line])) {
                throw std::runtime_error("Incorrect commands input detected.");
            }
        } catch (std::runtime_error &error) {
            logger.AddLog(error.what());
            FatalError();
        }
        filter_name = argv_[pos_in_command_line];
        logger.AddLog("Found filter " + filter_name + " at position " + std::to_string(pos_in_command_line + 1));
        OneFilter filter;
        filter.first = filter_name;
        ++pos_in_command_line;
        argument_pos = pos_in_command_line;
        for (const auto &parameter: filters_keywords_[filter_name]) {
            try {
                if (argument_pos >= argc_) {
                    throw std::runtime_error("Incorrect commands input detected.");
                }
            } catch (std::runtime_error &error) {
                logger.AddLog(error.what());
                FatalError();
            }

            try {
                if (!(std::regex_match(argv_[argument_pos], integer) &&
                      filters_keywords_[filter.first][parameter.first] == "int" ||
                      std::regex_match(argv_[argument_pos], floating) &&
                      filters_keywords_[filter.first][parameter.first] == "float")) {
                    throw std::runtime_error(
                            "Incorrect filter parameters input detected. Error occurred at:\nFilter position: " +
                            std::to_string(argument_pos) + ", filter name: " + filter.first + ".\nField \"" +
                            parameter.first + "\" wasn't initialised with " + parameter.second + ".");
                }
            } catch (std::runtime_error &error) {
                logger.AddLog(error.what());
                FatalError();
            }

            filter.second[parameter.first] = argv_[argument_pos];
            ++argument_pos;
            logger.AddLog("Assigning " + std::to_string(*argv_[argument_pos - 1]) + " in field " + parameter.first);
        }
        pos_in_command_line = argument_pos;
        filters_queue_.push_back(filter);
    }
    logger.AddLog("Current filter pipeline:");
    for (auto &item: filters_queue_) {
        logger.AddLog(item.first);
    }
}


std::string Parser::GetInputPath() {
    return file_paths_["input_file"];
}


std::string Parser::GetOutputPath() {
    return file_paths_["output_file"];
}


FiltersQueue Parser::GetFiltersQueue() {
    return filters_queue_;
}

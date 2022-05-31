#ifndef PROGRAM_STRING_PARSER_H
#define PROGRAM_STRING_PARSER_H


#include "logger.h"
#include "regex"


using OneFilterParameters = std::unordered_map<std::string, std::string>;
using OneFilter = std::pair<std::string, OneFilterParameters>;
using FiltersQueue = std::vector<OneFilter>;
using FiltersReferences = std::unordered_map<std::string, OneFilterParameters>;
using FilePaths = std::unordered_map<std::string, std::string>;


class Parser {
public:
    Parser(int argc, char *argv[]) : argc_(argc), argv_(argv) {}

    void AddFilePaths();
    void AddReferenceFilter(const std::string &keyword, const OneFilterParameters &params);
    void ParseCommandLine();

    std::string GetInputPath();
    std::string GetOutputPath();
    FiltersQueue GetFiltersQueue();

private:
    FilePaths file_paths_;
    FiltersReferences filters_keywords_;
    FiltersQueue filters_queue_;
    int argc_;
    char **argv_;
};


#endif //PROGRAM_STRING_PARSER_H

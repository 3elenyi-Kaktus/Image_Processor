#ifndef PROGRAM_LOGGER_H
#define PROGRAM_LOGGER_H


#include "fstream"
#include "ios"
#include "windows.h"
#include "iostream"


void FatalError();


class Logger {
public:
    Logger() : path("./error_log.txt") {}

    void AddLog(std::string log) const;
    void ClearLog() const;

private:
    std::string path;
};


#endif //PROGRAM_LOGGER_H

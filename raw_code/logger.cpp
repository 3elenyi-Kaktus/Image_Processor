#include "logger.h"


void FatalError() {
    std::cout << "Error in program execution process.\n"
                 "See the error_log file for detailed information.\n"
                 "Program is shutting down automatically in 10 seconds.\n" << std::endl;
    Sleep(10000);
    exit(-1);
}


void Logger::AddLog(const std::string log) const {
    std::fstream error_log(path, std::ios::app);
    error_log << log << std::endl;
    error_log.close();
}

void Logger::ClearLog() const {
    std::fstream error_log(path, std::ios::out);
    error_log.close();
}
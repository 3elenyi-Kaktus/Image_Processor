#include "filters_applier.h"


int main(int argc, char* argv[]) {
    Logger logger;
    try {
        Execute(argc, argv);
    } catch (std::exception& error){
        logger.AddLog(error.what());
        FatalError();
    } catch (...) {
        logger.AddLog("Undefined behaviour.\n");
        FatalError();
    }
    return 0;
}

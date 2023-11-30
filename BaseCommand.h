#include <string>

class BaseCommand {
public:
    virtual std::string execute(const char* cmd) = 0;
    virtual void displayHelp(const std::string& command) = 0;
};


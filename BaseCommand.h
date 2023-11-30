#include <string>
using namespace std;
class BaseCommand {
public:
    virtual string execute(const char* cmd) = 0;
    virtual void displayHelp(const string& command) = 0;
};


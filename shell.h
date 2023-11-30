
#include <iostream>
#include <vector>
#include <string>
#include "BaseCommand.h"
using namespace std;
class Shell : public BaseCommand {
public:
    void run();

    vector<string> tokenize(const string& input);

    void parseAndExecuteCommand(const string& input);

    bool containsHelpOption(const vector<string>& tokens);

    void changeDirectory(const vector<string>& tokens);
    //void listDirectoryContents(const vector<string>& tokens, const string& command);
    void listDirectoryContents(const vector<string>& tokens);

    string execute(const char* cmd) override;

    void displayHelp(const string& command) override;

    void listSubdirectoryContents(const string& directory,
                                  bool showHidden,
                                  bool reverseOrder,
                                  bool recursive);

    void moveFile(const vector<string>& tokens, const string& command);

    void removeFile(const vector<string>& tokens, const string& command);

    void copyFile(const vector<string>& tokens, const string&);

    void executeShellScript(const string& scriptFileName);
};


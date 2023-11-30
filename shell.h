
#include <iostream>
#include <vector>
#include <string>
#include "BaseCommand.h"

class Shell : public BaseCommand {
public:
    void run();

    std::vector<std::string> tokenize(const std::string& input);

    void parseAndExecuteCommand(const std::string& input);

    bool containsHelpOption(const std::vector<std::string>& tokens);

    void changeDirectory(const std::vector<std::string>& tokens);

    void listDirectoryContents(const std::vector<std::string>& tokens);

    std::string execute(const char* cmd) override;

    void displayHelp(const std::string& command) override;

    void listSubdirectoryContents(const std::string& directory,
                                  bool showHidden,
                                  bool reverseOrder,
                                  bool recursive);

    void moveFile(const std::vector<std::string>& tokens, const std::string& command);

    void removeFile(const std::vector<std::string>& tokens, const std::string& command);

    void copyFile(const std::vector<std::string>& tokens, const std::string&);

};


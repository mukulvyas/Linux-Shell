#include <iostream>
#include <sstream>
#include <cstdlib>
#include <unistd.h>

class SimpleShell {
public:
    void run() {
        std::string input;
        while (true) {
            std::cout << "Enter a command (or 'exit' to quit): ";
            std::getline(std::cin, input);

            if (input == "exit") {
                break;
            }

            executeCommand(input);
        }
    }

private:
    void executeCommand(const std::string& command) {
        if (command.substr(0, 3) == "cd") {
            // If the command is 'cd', change the working directory
            changeDirectory(command.substr(3));
        } else {
            int result = std::system(command.c_str());
            if (result == 0) {
                std::cout << "Command executed successfully." << std::endl;
            } else {
                std::cerr << "Command failed to execute." << std::endl;
            }
        }
    }

    void changeDirectory(const std::string& path) {
        std::string trimmedPath = path;
        trimmedPath.erase(0, trimmedPath.find_first_not_of(" \t\n\r\f\v"));
        trimmedPath.erase(trimmedPath.find_last_not_of(" \t\n\r\f\v") + 1);

        if (trimmedPath.empty()) {
            std::cerr << "Invalid usage of cd command." << std::endl;
            return;
        }

        if (chdir(trimmedPath.c_str()) == 0) {
            char* currentDirectory = getcwd(NULL, 0);
            std::cout << "Directory changed to: " << currentDirectory << std::endl;
            free(currentDirectory);
        } else {
            std::cerr << "Failed to change directory." << std::endl;
        }
    }
};

int main() {
    SimpleShell shell;
    shell.run();

    return 0;
}

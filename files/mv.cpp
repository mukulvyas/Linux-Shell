#include <iostream>
#include <cstdlib>
#include <string>

class CommandExecutor {
public:
    CommandExecutor(const std::string& input) : command(input) {}

    void execute() {
        int result = system(command.c_str());

        if (result == 0) {
            std::cout << "Command executed successfully." << std::endl;
        } else {
            std::cerr << "Error executing command. Exit code: " << result << std::endl;
        }
    }

private:
    std::string command;
};

int main() {
    while (true) {
        // Get user input for the command
        std::cout << "MyShell> ";
        std::string input;
        std::getline(std::cin, input);

        // Check for the exit condition
        if (input == "exit") {
            std::cout << "Exiting MyShell." << std::endl;
            break;
        }

        // Create an instance of the CommandExecutor class
        CommandExecutor commandExecutor(input);

        // Execute the command with user input
        commandExecutor.execute();
    }

    return 0;
}

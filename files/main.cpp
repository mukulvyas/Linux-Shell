#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class Shell {
public:
    Shell() {
        currentPath = fs::current_path();
    }

    void run() {
        std::string input;
        while (true) {
            std::cout << currentPath << "> ";
            std::getline(std::cin, input);
            
            if (input == "exit") {
                break;
            }

            processCommand(input);
        }
    }

private:
    fs::path currentPath;

    void processCommand(const std::string& command) {
        std::vector<std::string> tokens = tokenize(command);

        if (tokens.empty()) {
            return;
        }

        std::string cmd = tokens[0];

        if (cmd == "cd") {
            changeDirectory(tokens);
        } else if (cmd == "mv") {
            moveFile(tokens);
        } else if (cmd == "rm") {
            removeFile(tokens);
        } else if (cmd == "ls") {
            listFiles(tokens);
        } else if (cmd == "cp") {
            copyFile(tokens);
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
        }
    }

    std::vector<std::string> tokenize(const std::string& input) {
        std::vector<std::string> tokens;
        size_t pos = 0;
        size_t found = input.find(' ');

        while (found != std::string::npos) {
            tokens.push_back(input.substr(pos, found - pos));
            pos = found + 1;
            found = input.find(' ', pos);
        }

        tokens.push_back(input.substr(pos));
        return tokens;
    }

    void changeDirectory(const std::vector<std::string>& tokens) {
        if (tokens.size() == 1) {
            std::cout << "Usage: cd <directory>" << std::endl;
        } else {
            fs::path newDir(tokens[1]);
            if (fs::exists(newDir) && fs::is_directory(newDir)) {
                currentPath = fs::absolute(newDir);
            } else {
                std::cout << "Directory does not exist: " << tokens[1] << std::endl;
            }
        }
    }

    void moveFile(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            std::cout << "Usage: mv <source> <destination>" << std::endl;
            return;
        }

        fs::path source(tokens[1]);
        fs::path destination(tokens[2]);

        try {
            fs::rename(source, destination);
            std::cout << "File moved successfully." << std::endl;
        } catch (const fs::filesystem_error& e) {
            std::cout << "Error moving file: " << e.what() << std::endl;
        }
    }

    void removeFile(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Usage: rm <file>" << std::endl;
            return;
        }

        fs::path file(tokens[1]);

        try {
            if (fs::remove(file)) {
                std::cout << "File removed successfully." << std::endl;
            } else {
                std::cout << "Error removing file." << std::endl;
            }
        } catch (const fs::filesystem_error& e) {
            std::cout << "Error removing file: " << e.what() << std::endl;
        }
    }

    void listFiles(const std::vector<std::string>& tokens) {
        if (tokens.size() > 1) {
            std::cout << "Usage: ls" << std::endl;
            return;
        }

        for (const auto& entry : fs::directory_iterator(currentPath)) {
            std::cout << entry.path().filename().string() << std::endl;
        }
    }

    void copyFile(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            std::cout << "Usage: cp <source> <destination>" << std::endl;
            return;
        }

        fs::path source(tokens[1]);
        fs::path destination(tokens[2]);

        try {
            fs::copy(source, destination, fs::copy_options::recursive);
            std::cout << "File copied successfully." << std::endl;
        } catch (const fs::filesystem_error& e) {
            std::cout << "Error copying file: " << e.what() << std::endl;
        }
    }
};

int main() {
    Shell myShell;
    myShell.run();

    return 0;
}

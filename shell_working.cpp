#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm> 
using namespace std;
namespace fs = filesystem;


class BaseCommand {
public:
  virtual string execute(const char* cmd) = 0;
  virtual void displayHelp(const string& command) = 0;
};


class Shell: public BaseCommand {
public:

    

    void run() {
        while (true) {
            cout << "MyShell> ";
            string input;
            getline(cin, input);

            if (input == "exit") {
                break;
            }

            parseAndExecuteCommand(input);
        }
    }

public:

    vector<string> tokenize(const string& input) {
        vector<string> tokens;
        size_t pos = 0;
        size_t found = input.find(' ');

        while (found != string::npos) {
            tokens.push_back(input.substr(pos, found - pos));
            pos = found + 1;
            found = input.find(' ', pos);
        }

        tokens.push_back(input.substr(pos));
        return tokens;
    }

    void parseAndExecuteCommand(const string& input) {
    vector<string> tokens = tokenize(input);
    if (tokens.empty()) {
        return;
    }

    string command = tokens[0];

    if (command == "cd") {
        if (containsHelpOption(tokens)) {
            displayHelp("cd");
        } else {
           changeDirectory(tokens);
        }
        
    } else if (command == "ls") {
        if (containsHelpOption(tokens)) {
            displayHelp("ls");
        } else {
            listDirectoryContents(tokens);
        }
    } else if (command == "mv") {
        if (containsHelpOption(tokens)) {
            displayHelp("mv");
        } else {
            moveFile(tokens,input);
        }
    } else if (command == "rm") {
        if (containsHelpOption(tokens)) {
            displayHelp("rm");
        } else {
            removeFile(tokens,input);
        }
    } else if (command == "cp") {
        if (containsHelpOption(tokens)) {
            displayHelp("cp");
        } else {
            copyFile(tokens,input);
        }
    } else {
        cout << "Unknown command: " << command << endl;
    }
}

bool containsHelpOption(const vector<string>& tokens) {
    for (const auto& token : tokens) {
        if (token == "--help") {
            return true;
        }
    }
    return false;
}


    void changeDirectory(const vector<string>& tokens) {
    if (tokens.size() > 1) {
        fs::path targetPath(tokens[1]);

        // Options
        bool followSymlinks = true;  // Default behavior (equivalent to -L)
        bool usePhysicalStructure = false;  // Default behavior (equivalent to -P)

        // Check for options
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (tokens[i] == "-L") {
                followSymlinks = true;
                usePhysicalStructure = false;
            } else if (tokens[i] == "-P") {
                followSymlinks = false;
                usePhysicalStructure = true;
            } else if (tokens[i] == "-e") {
                // Handle -e option if needed
            } else {
                cerr << "Unknown option: " << tokens[i] << endl;
                return;
            }
        }

        // Apply options
        if (followSymlinks) {
            if (fs::exists(targetPath) && fs::is_directory(targetPath)) {
                fs::current_path(targetPath);
            } else {
                cout << "Directory not found: " << targetPath << endl;
            }
        } else if (usePhysicalStructure) {
            try {
                fs::current_path(fs::canonical(targetPath));
            } catch (const fs::filesystem_error& e) {
                cerr << "Error determining physical directory structure: " << e.what() << endl;
                // Handle the error as needed
            }
        }
    } else {
        // Handle the case where not enough arguments are provided
        displayHelp("cd");
    }
}


    void listDirectoryContents(const vector<string>& tokens) {
    bool showHidden = false;
    bool showCount = false;
    bool reverseOrder = false;
    bool recursive = false;

    for (size_t i = 1; i < tokens.size(); ++i) {
        if (tokens[i] == "-a" || tokens[i] == "--all") {
            showHidden = true;
        } else if (tokens[i] == "-c" || tokens[i] == "--count") {
            showCount = true;
        } else if (tokens[i] == "-r" || tokens[i] == "--reverse") {
            reverseOrder = true;
        } else if (tokens[i] == "-R" || tokens[i] == "--recursive") {
            recursive = true;
        } else if (tokens[i] == "--help") {
            displayHelp("ls");
            return;
        } else {
            cout << "Unknown option: " << tokens[i] << endl;
            return;
        }
    }

    fs::directory_iterator end_itr;
    vector<string> fileNames;

    for (fs::directory_iterator itr(fs::current_path()); itr != end_itr; ++itr) {
        if (!showHidden && itr->path().filename().string()[0] == '.') {
            continue; // Skip hidden files
        }

        fileNames.push_back(itr->path().filename().string());
    }

    if (reverseOrder) {
        sort(fileNames.rbegin(), fileNames.rend());
    } else {
        sort(fileNames.begin(), fileNames.end());
    }

    if (recursive) {
        for (const auto& fileName : fileNames) {
            cout << fileName << " ";
            if (fs::is_directory(fileName)) {
                // Recursive call to list subdirectory contents
                listSubdirectoryContents(fileName, showHidden, reverseOrder, recursive);
            }
        }
    } else {
        for (const auto& fileName : fileNames) {
            cout << fileName << " ";
        }
    }

    if (showCount) {
        cout << "\nTotal: " << fileNames.size() << " items" << endl;
    }

    cout << endl;
}

string execute(const char* cmd ) override {
    char buffer[128];
    string result = "";
    
    // Open a pipe to the command
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        cerr << "Error executing command." << endl;
        return "";
    }

    // Read the command output and append it to the result string
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr)
            result += buffer;
    }

    // Close the pipe
    pclose(pipe);

    return result;
    }

void displayHelp(const string& command) override {

    if (command == "cd") {
        cout << "cd: cd [-L|[-P [-e]] [-@]] [dir]\n";
        cout << "    Change the shell working directory.\n\n";
        cout << "    Change the current directory to DIR.  The default DIR is the value of the\n";
        cout << "    HOME shell variable.\n\n";
        cout << "    The variable CDPATH defines the search path for the directory containing\n";
        cout << "    DIR.  Alternative directory names in CDPATH are separated by a colon (:).\n";
        cout << "    A null directory name is the same as the current directory.  If DIR begins\n";
        cout << "    with a slash (/), then CDPATH is not used.\n\n";
        cout << "    If the directory is not found, and the shell option `cdable_vars' is set,\n";
        cout << "    the word is assumed to be a variable name.  If that variable has a value,\n";
        cout << "    its value is used for DIR.\n\n";
        cout << "    Options:\n";
        cout << "      -L    force symbolic links to be followed: resolve symbolic\n";
        cout << "            links in DIR after processing instances of `..'\n";
        cout << "      -P    use the physical directory structure without following\n";
        cout << "            symbolic links: resolve symbolic links in DIR before\n";
        cout << "            processing instances of `..'\n";
        cout << "      -e    if the -P option is supplied, and the current working\n";
        cout << "            directory cannot be determined successfully, exit with\n";
        cout << "            a non-zero status\n";
        cout << "      -@   on systems that support it, present a file with extended\n";
        cout << "            attributes as a directory containing the file attributes\n\n";
        cout << "    The default is to follow symbolic links, as if `-L' were specified.\n";
        cout << "    `..' is processed by removing the immediately previous pathname component\n";
        cout << "    back to a slash or the beginning of DIR.\n\n";
        cout << "    Exit Status:\n";
        cout << "    Returns 0 if the directory is changed, and if $PWD is set successfully when\n";
        cout << "    -P is used; non-zero otherwise.\n";      
            
        } else if (command == "ls") {
            string output = executeCommand("ls --help");
            cout << "\nOutput:\n" << output << endl;
        } else if (command == "mv") {
            string output = executeCommand("mv --help");
            cout << "\nOutput:\n" << output << endl;
            
        } else if (command == "rm") {
            string output = executeCommand("rm --help");
            cout << "\nOutput:\n" << output << endl;
            
        } else if (command == "cp") {
            string output = executeCommand("cp --help");
            cout << "\nOutput:\n" << output << endl;
            
        }
       
    }

void listSubdirectoryContents(const string& directory,
                               bool ,
                               bool ,
                               bool ) {
    fs::current_path(directory);
    cout << "\n" << directory << ":\n";
    listDirectoryContents(vector<string>{ "-a", "-c", "-r", "-R" });
    fs::current_path("..");
}


void moveFile(const vector<string>& tokens, const string& command) {
    // Check if there are at least two arguments (source and destination)
    if (tokens.size() < 3) {
        cerr << "Usage: mv [options] source destination" << endl;
        return;
    }

    // Construct the base "mv" command
    string mvCommand = "mv";
    int result;
    // Handle each option separately
    for (size_t i = 1; i < tokens.size() - 2; ++i) {
        const string& option = tokens[i];
        if (option == "-f" || option == "--force") {
             result =system(command.c_str());
            //mvCommand += " -f";
        } else if (option == "-n" || option == "--no-clobber") {
            result =  system(command.c_str());

            //mvCommand += " -n";
        } else if (option == "-i" || option == "--interactive") {
           result = system(command.c_str());

            //mvCommand += " -i";
        } else {
            cerr << "Unknown option: " << option << endl;
            return;
        }
    }

    // Execute the constructed "mv" command
    //int result = system(mvCommand.c_str());

    // Check the result of the command execution
    if (result == 0) {
        cout << "Command executed successfully." << endl;
    } else {
        cerr << "Error executing command. Exit code: " << result << endl;
    }
}

    void removeFile(const vector<string>& tokens, const string& command) {
    bool forceRemove = false;
    bool recursiveRemove = false;
    bool removeEmptyDirs = false;
    int result;
    for (size_t i = 1; i < tokens.size(); ++i) {
        //cout<<tokens[i];
        if (tokens[i] == "-f" || tokens[i] == "--force") {

            forceRemove = true;
            
            break;
        } else if (tokens[i] == "-r" || tokens[i] == "-R" || tokens[i] == "--recursive") {
            recursiveRemove = true;
            //result = system(command.c_str());
            break;
        } else if (tokens[i] == "-d" || tokens[i] == "--dir") {
            removeEmptyDirs = true;
            //result = system(command.c_str());
            break;
        } else if (tokens[i] == "--help") {
            displayHelp("rm");
            return;
        } 
    }

    if (tokens.size() > 1) {
        try {
            if (forceRemove) {
                if (recursiveRemove) {
                    result = system(command.c_str());
                } else {
                    if (removeEmptyDirs) {
                        result = system(command.c_str());
                    } else {
                        result = system(command.c_str());
                    }
                }
            } else {
                if (recursiveRemove) {
                    result = system(command.c_str());
                } else {
                    if (removeEmptyDirs) {
                        result = system(command.c_str());
                    } else {
                        result = system(command.c_str());
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            cerr << "Error removing file/directory: " << e.what() << endl;
            // Handle the error as needed
        }
    } else {
        cout<<"Please try with proper command!!"<<endl;
    }

    if (result == 0) {
        cout << "Command executed successfully." << endl;
    } else {
        cerr << "Error executing command. Exit code: " << result << endl;
    }
}


void copyFile(const vector<string>& tokens, const string&) {
    bool recursiveCopy = false;
    bool forceOverwrite = false;
    bool updateOnly = false;
    int result = 0; // Initialize result

    for (size_t i = 1; i < tokens.size(); ++i) {
        if (tokens[i] == "-r" || tokens[i] == "--recursive") {
            recursiveCopy = true;
        } else if (tokens[i] == "-i" || tokens[i] == "--interactive") {
            forceOverwrite = true;
        } else if (tokens[i] == "-u" || tokens[i] == "--update") {
            updateOnly = true;
        } else if (tokens[i] == "--help") {
            displayHelp("cp");
            return;
        }
    }

    if (tokens.size() >= 4) {

        string source = tokens[2];
        string destination = tokens[3];

        string cpCommand = "cp";

       
        if (recursiveCopy) {
            cpCommand += " -r";
        }
        if (forceOverwrite) {
            cpCommand += " -i";
        }
        if (updateOnly) {
            cpCommand += " -u";
        }


        cpCommand += " " + source + " " + destination;


        result = system(cpCommand.c_str());
    } else {
        cout << "Please enter correct command!!" << endl;
    }

    if (result == 0) {
        cout << "Command executed successfully." << endl;
    } else {
        cerr << "Error executing command. Exit code: " << result << endl;
    }
}


    string executeCommand(const char* cmd) {
    char buffer[128];
    string result = "";
    
    // Open a pipe to the command
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        cerr << "Error executing command." << endl;
        return "";
    }

    // Read the command output and append it to the result string
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr)
            result += buffer;
    }

    // Close the pipe
    pclose(pipe);

    return result;
}

    // void displayHelp(const string& command) {
    //     //cout << "Usage: ";

    //    if (command == "cd") {
    //     cout << "cd: cd [-L|[-P [-e]] [-@]] [dir]\n";
    //     cout << "    Change the shell working directory.\n\n";
    //     cout << "    Change the current directory to DIR.  The default DIR is the value of the\n";
    //     cout << "    HOME shell variable.\n\n";
    //     cout << "    The variable CDPATH defines the search path for the directory containing\n";
    //     cout << "    DIR.  Alternative directory names in CDPATH are separated by a colon (:).\n";
    //     cout << "    A null directory name is the same as the current directory.  If DIR begins\n";
    //     cout << "    with a slash (/), then CDPATH is not used.\n\n";
    //     cout << "    If the directory is not found, and the shell option `cdable_vars' is set,\n";
    //     cout << "    the word is assumed to be a variable name.  If that variable has a value,\n";
    //     cout << "    its value is used for DIR.\n\n";
    //     cout << "    Options:\n";
    //     cout << "      -L    force symbolic links to be followed: resolve symbolic\n";
    //     cout << "            links in DIR after processing instances of `..'\n";
    //     cout << "      -P    use the physical directory structure without following\n";
    //     cout << "            symbolic links: resolve symbolic links in DIR before\n";
    //     cout << "            processing instances of `..'\n";
    //     cout << "      -e    if the -P option is supplied, and the current working\n";
    //     cout << "            directory cannot be determined successfully, exit with\n";
    //     cout << "            a non-zero status\n";
    //     cout << "      -@   on systems that support it, present a file with extended\n";
    //     cout << "            attributes as a directory containing the file attributes\n\n";
    //     cout << "    The default is to follow symbolic links, as if `-L' were specified.\n";
    //     cout << "    `..' is processed by removing the immediately previous pathname component\n";
    //     cout << "    back to a slash or the beginning of DIR.\n\n";
    //     cout << "    Exit Status:\n";
    //     cout << "    Returns 0 if the directory is changed, and if $PWD is set successfully when\n";
    //     cout << "    -P is used; non-zero otherwise.\n";      
            
    //     } else if (command == "ls") {
    //         string output = executeCommand("ls --help");
    //         cout << "\nOutput:\n" << output << endl;
    //     } else if (command == "mv") {
    //         string output = executeCommand("mv --help");
    //         cout << "\nOutput:\n" << output << endl;
            
    //     } else if (command == "rm") {
    //         string output = executeCommand("rm --help");
    //         cout << "\nOutput:\n" << output << endl;
            
    //     } else if (command == "cp") {
    //         string output = executeCommand("cp --help");
    //         cout << "\nOutput:\n" << output << endl;
            
    //     }
    // }
};

int main() {
    Shell myShell;
    myShell.run();
    return 0;
}

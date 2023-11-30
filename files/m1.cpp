#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

// Function to execute a Linux command and capture its output
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

int main() {
    cout << "Enter a Linux command: ";
    string userCommand;
    getline(cin, userCommand);

    // Execute the user-specified command and display the output
    string output = executeCommand(userCommand.c_str());
    
    cout << "\nOutput:\n" << output << endl;

    return 0;
}

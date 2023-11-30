#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <thread>
#include <functional>
#include <mutex>

using namespace std;

class BaseCommand {
public:
    virtual string execute(const char* cmd) = 0;
    virtual void displayHelp() const = 0;
    virtual ~BaseCommand() {}
};

class ShellCommand : public BaseCommand {
public:
    string execute(const char* cmd) override {
        char buffer[128];
        string result = "";

        FILE* pipe = popen(cmd, "r");
        if (!pipe) {
            cerr << "Error executing command." << endl;
            return "";
        }

        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != nullptr)
                result += buffer;
        }

        pclose(pipe);

        return result;
    }

    void displayHelp() const override {
        cout << "This is a basic shell command. No specific help available." << endl;
    }

    // Make ShellCommand callable
    void operator()() const {
        cout << "Executing ShellCommand..." << endl;
    }
};

class ThreadPool {
public:
    vector<thread> threads;
    mutex mtx;

public:
    ThreadPool(int numThreads) : threads(numThreads) {}

    void executeInParallel(const function<void()>& func) {
        lock_guard<mutex> lock(mtx);

        for (auto& thread : threads) {
            thread = std::thread(func);
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }
};

class MyShell {
private:
    ThreadPool threadPool;
    ShellCommand shellCommand;

public:
    MyShell(int numThreads) : threadPool(numThreads) {}

    void run() {
        while (true) {
            cout << "MyShell> ";
            string input;
            getline(cin, input);

            if (input == "exit") {
                break;
            }

            processCommand(input);
        }
    }

private:
    void processCommand(const string& input) {
        vector<string> tokens = tokenize(input);
        if (tokens.empty()) {
            return;
        }

        string command = tokens[0];
        if (command == "help") {
            shellCommand.displayHelp();
        } else {
            executeCommand(tokens);
        }
    }

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

    void executeCommand(const vector<string>& tokens) {
        string cmd = join(tokens, " ");
        // Use bind to create a callable object from shellCommand
        auto func = bind(&ShellCommand::operator(), &shellCommand);
        threadPool.executeInParallel(func);
    }

    string join(const vector<string>& tokens, const string& delimiter) {
        string result;
        if (!tokens.empty()) {
            result = tokens[0];
            for (size_t i = 1; i < tokens.size(); ++i) {
                result += delimiter + tokens[i];
            }
        }
        return result;
    }
};

int main() {
    int numThreads = thread::hardware_concurrency();
    if (numThreads == 0) {
        cerr << "Unable to determine the number of CPU cores. Defaulting to 1 thread." << endl;
        numThreads = 1;
    }

    MyShell myShell(numThreads);
    myShell.run();

    return 0;
}


#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm> 
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstring>
#include <memory>
#include <thread>
#include <mutex>
using namespace std;

class Command_first
{
public:
    virtual void execute() = 0;
    virtual ~Command_first() {}
};

class Exit : public Command_first
{
public:
    void execute() override
    {
        exit(0);
    }
};

class Cd : public Command_first
{
private:
    char cwd[256];

public:
    Cd(const vector<string> &args)
    {
        if (chdir(args[1].c_str()) != 0)
        {
            perror("chdir");
        }
        else
        {
            getcwd(cwd, sizeof(cwd)); // Update the current working directory
        }
    }

    void execute() override {}
};

class ProcessExecutor : public Command_first
{
private:
    vector<string> args;

public:
    ProcessExecutor(const vector<string> &args) : args(args) {}

    void execute() override
    {
        pid_t pid, wait_pid;
        int status;

        pid = fork();

        if (pid < 0)
        {
            cerr << "Fork failed." << endl;
        }
        else if (pid == 0)
        {
            // Child process
            char **argv = new char *[args.size() + 1];
            for (size_t i = 0; i < args.size(); ++i)
            {
                argv[i] = const_cast<char *>(args[i].c_str());
            }
            argv[args.size()] = nullptr;

            if (execvp(argv[0], argv) == -1)
            {
                cerr << "Command not found: " << argv[0] << endl;
                delete[] argv;
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // Parent process
            do
            {
                wait_pid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));

            // Print something to indicate successful execution along with thread ID
            cout << "Command executed successfully using multiple threads! Thread ID: " << this_thread::get_id() << endl;
        }
    }
};

class Thread
{
private:
    vector<thread> threads;
    mutex mtx;

public:
    // Default constructor
    Thread() {}

    Thread(int numThreads) : threads(numThreads)
    {
    }

    // Delete the move assignment operator
    Thread &operator=(Thread &&) = delete;

    void executeInParallel(shared_ptr<Command_first> command)
    {
        lock_guard<mutex> lock(mtx);

        for (auto &thread : threads)
        {
            thread = std::thread(&Command_first::execute, command);
        }

        for (auto &thread : threads)
        {
            thread.join();
        }
    }
};

class BaseCommand
{
public:
    virtual string execute(const char *cmd) = 0;
    virtual void displayHelp(const string &command) = 0;
};

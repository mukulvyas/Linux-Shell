#include "BaseCommand.h"
using namespace std;

class Shell : public BaseCommand {
public:

    char cwd[256]; // Buffer to store the current working directory
    Thread threadPool;

    unique_ptr<Command_first> createCommand(const vector<string> &args)
    {
        const char *command = args[0].c_str();

        if (args.size() == 1 && strcmp(command, "exit") == 0)
        {
            return make_unique<Exit>();
        }

        if (args.size() == 2 && strcmp(command, "cd") == 0)
        {
            return make_unique<Cd>(args);
        }

        return make_unique<ProcessExecutor>(args);
    }

    int calculateOptimalThreads(int availableCores, int workload)
    {
        // Adjust the number of threads based on workload and available cores
        // This is a simplified example, you may need more sophisticated logic
        int optimalThreads = min(availableCores, workload);
        return optimalThreads > 0 ? optimalThreads : 1; // Ensure at least one thread
    }

    Shell()
    {
        // Get the number of available CPU cores
        int numCores = thread::hardware_concurrency();
        if (numCores == 0)
        {
            cerr << "Unable to determine the number of CPU cores. Defaulting to 1 thread." << endl;
            numCores = 1;
        }

        // Calculate the initial number of threads based on available cores
        int initialThreads = calculateOptimalThreads(numCores, 1);

        // Use emplace_back to construct a new ThreadPool object without move assignment
        threadPool.~Thread();
        new (&threadPool) Thread(initialThreads);

        // Initialize the current working directory
        getcwd(cwd, sizeof(cwd));
    }
    void run();

    vector<string> tokenize(const string& input);

    void parseAndExecuteCommand(const string& input);

    bool containsHelpOption(const vector<string>& tokens);

    void changeDirectory(const vector<string>& tokens);

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


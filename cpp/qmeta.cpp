#include <iostream>
#include <sstream>
#include <vector>
//#include <system>

std::vector<std::string> const availableCommands = {"root"};

int main(int argv, char** argc) {
    // Complete yolo for now, prob switch to boost to parse args.
    if (argv < 2) {
        system("git-meta");
    } else if (std::find(availableCommands.cbegin(), availableCommands.cend(),
              (argc[1])) != availableCommands.cend()) { // TODO: Switch to binary_search
        std::cout << "Not implemented yet: " << argc[1] << std::endl;
    } else {
        std::stringstream gitMetaCommand;
        gitMetaCommand << "git-meta";
        for (int i = 1; i < argv; ++i) {
            gitMetaCommand << " " << argc[i];
        }
        system(gitMetaCommand.str().c_str());
    }
}

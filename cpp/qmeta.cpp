#include <iostream>
#include <sstream>
#include <vector>
//#include <system>
#include <lib/util/git_util.h>

std::vector<std::string> const availableCommands = {"root"};
int const commandPos = 1;

int main(int argv, char** argc) {
    // Complete yolo for now, prob switch to boost to parse args.
    if (argv < 2) {
        system("git-meta");
    } else if (std::find(availableCommands.cbegin(), availableCommands.cend(),
              (argc[commandPos])) != availableCommands.cend()) {
        std::string const gitRootDir
            = qmeta::lib::util::getContainingGitDir(boost::filesystem::current_path()).string();
        std::cout << gitRootDir << std::endl;
    } else {
        std::stringstream gitMetaCommand;
        gitMetaCommand << "git-meta";
        for (int i = 1; i < argv; ++i) {
            gitMetaCommand << " " << argc[i];
        }
        system(gitMetaCommand.str().c_str());
    }
}

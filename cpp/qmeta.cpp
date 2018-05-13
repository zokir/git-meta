#include <iostream>
#include <sstream>
#include <vector>
//#include <system>
#include <lib/util/git_util.h>

#include <boost/program_options.hpp>

std::vector<std::string> const availableCommands = {"root"};
int const commandPos = 1;

int handleSupportedCommands(int argc, char* argv[]) {
    namespace po = boost::program_options;
    try {
        po::options_description desc{"Options"};
        desc.add_options()
            ("help,h", "Show this help message and exit")
            ("root", "Print the root directory of the meta-repo.");

            po::positional_options_description pos_desc;
            pos_desc.add("root", -1);
            po::command_line_parser parser{argc, argv};
            parser.options(desc).positional(pos_desc).allow_unregistered();
            po::parsed_options parsedOptions = parser.run();

            po::variables_map vm;
            store(parsedOptions, vm);
            po::notify(vm);

            if (vm.count("help")) {
                std::cout << desc << std::endl;
            } else if (vm.count("root")) {
                boost::filesystem::path const gitRootPath
                    = qmeta::lib::util::getContainingGitDir(boost::filesystem::current_path());
                std::cout << gitRootPath.string() << std::endl;

                // if (relative) {
                //std::cout
                //    << boost::filesystem::relative(boost::filesystem::current_path(),gitRootPath).string()
                //    << std::endl;
                //
            }
    } catch (po::error const& exception) {
        std::cerr << exception.what() << std::endl;
        return -1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    // Complete yolo for now, prob switch to boost to parse args.
    if (argc < 2) {
        return system("git-meta");
    } else if (std::find(availableCommands.cbegin(), availableCommands.cend(),
              (argv[commandPos])) != availableCommands.cend()) {
        return handleSupportedCommands(argc, argv);
    } else {
        std::stringstream gitMetaCommand;
        gitMetaCommand << "git-meta";
        for (int i = 1; i < argc; ++i) {
            gitMetaCommand << " " << argv[i];
        }
        return system(gitMetaCommand.str().c_str());
    }
}

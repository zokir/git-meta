#include <iostream>
#include <sstream>
#include <vector>

#include <boost/program_options.hpp>

#include <lib/core/error.h>
#include <lib/util/git_util.h>

std::vector<std::string> const availableCommands = {"root"};
int const commandPos = 1;



int handleSupportedCommands(int argc, char* argv[]) {
    namespace po = boost::program_options;
    try {
        po::options_description visible{"Options"};
        visible.add_options()
            ("help,h", "Show this help message and exit")
            ("version,v", "Print version");

        po::options_description hidden{"Hidden Options"};
        hidden.add_options()
            ("command", po::value<std::string>(), "command to execute");

        po::positional_options_description pos_desc;
        pos_desc.add("command", -1);

        po::options_description allOptions("All options");
        allOptions.add(visible).add(hidden);

        po::variables_map vm;
        po::parsed_options parsed = po::command_line_parser(argc, argv).
            options(allOptions).
            positional(pos_desc).
            allow_unregistered().
            run();

        po::store(parsed, vm);
        if (!vm.count("command")) {
            std::cout << visible << std::endl;
            return 0;
        }

			std::string cmd = vm["command"].as<std::string>();
			if (cmd == "root")
			{
				po::options_description root_desc("Optional arguments");
				root_desc.add_options()
                    ("help,h", "Show this help message and exit")
                    ("relative,r",
                                   "Print the relative path between current directory and root."
                                   "E.g., 'cd $(git meta root)'; cd a/b/c; git meta root -r'"
                                   "will print 'a/b/c'.");

				// Collect all the unrecognized options from the first pass. This will include the
				// (positional) command name, so we need to erase that.
				std::vector<std::string> opts = po::collect_unrecognized(parsed.options, po::include_positional);
				opts.erase(opts.begin());

				// Parse again...
				po::store(po::command_line_parser(opts).options(root_desc).run(), vm);

                if (vm.count("help")) {
                    std::string const descriptionMessage =
                        "From within any subdirectory of a meta-repository -- including when the\n"
                        "working directory is in a submodule -- print the root of the working\n"
                        "directory of that meta-repository.\n";
                    std::cout << descriptionMessage << "\n" << root_desc << std::endl;
                    return 0;
                }

                boost::filesystem::path const gitRootPath
                    = qmeta::lib::util::getContainingGitDir(boost::filesystem::current_path());

                if (vm.count("relative")) { // TODO: do bool switches
                    std::cout
                        << boost::filesystem::relative(boost::filesystem::current_path(),gitRootPath).string()
                        << std::endl;
                } else {
                    std::cout << gitRootPath.string() << std::endl;
                }

			}
    } catch (qmeta::lib::core::NotInGitRepoError const& exception) {
        std::cerr << exception.what() << std::endl;
        return -1;
    } catch (po::error const& exception) {
        std::cerr << exception.what() << std::endl;
        return -1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    return handleSupportedCommands(argc, argv);
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

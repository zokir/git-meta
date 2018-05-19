#include <iostream>
#include <sstream>
#include <vector>

#include <boost/program_options.hpp>

#include <lib/core/error.hpp>
#include <lib/cmd/root.hpp>

std::vector<std::string> const availableCommands = {"root"};
int const commandPos = 1;

int handleSupportedCommands(int argc, char *argv[]) {
  namespace po = boost::program_options;
  try {
    po::options_description visible{"Options"};
    visible.add_options()("help,h", "Show this help message and exit")(
        "version,v", "Print version");

    po::options_description hidden{"Hidden Options"};
    hidden.add_options()("command", po::value<std::string>(),
                         "command to execute");

    po::positional_options_description pos_desc;
    pos_desc.add("command", -1);

    po::options_description allOptions("All options");
    allOptions.add(visible).add(hidden);

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc, argv)
                                    .options(allOptions)
                                    .positional(pos_desc)
                                    .allow_unregistered()
                                    .run();

    po::store(parsed, vm);
    if (!vm.count("command")) {
      std::cout << visible << std::endl;
      return 0;
    }

    std::string cmd = vm["command"].as<std::string>();

    // Collect all the unrecognized options from the first pass. This will
    // include the (positional) command name, so we need to erase that.
    std::vector<std::string> opts =
        po::collect_unrecognized(parsed.options, po::include_positional);
    opts.erase(opts.begin());

    if (vm.count("help")) {
        // [cmd]->printHelp
    }

    if (cmd == "root") {
        qmeta::lib::cmd::Root rootCmd;
        if (vm.count("help")) {
            rootCmd.printHelp();
        } else {
            rootCmd.execute(opts);
        }
    }

  } catch (qmeta::lib::core::NotInGitRepoError const &exception) {
    std::cerr << exception.what() << std::endl;
    return -1;
  } catch (po::error const &exception) {
    std::cerr << exception.what() << std::endl;
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
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

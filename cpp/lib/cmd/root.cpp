#include <lib/cmd/root.hpp>
#include <lib/util/git_util.hpp>

#include <boost/filesystem.hpp>

#include <iostream>

qmeta::lib::cmd::Root::Root():
    Command("Print the root directory of the meta-repo."),
    m_optDesc("Optional arguments")
{
      m_optDesc.add_options()("help,h", "Show this help message and exit")(
          "relative,r",
          "Print the relative path between current directory and root."
          "E.g., 'cd $(git meta root)'; cd a/b/c; git meta root -r'"
          "will print 'a/b/c'.");

}

void qmeta::lib::cmd::Root::printHelp() const {
        std::string const descriptionMessage =
                      "From within any subdirectory of a meta-repository -- including "
                      "when the\n"
                      "working directory is in a submodule -- print the root of the "
                      "working\n"
                      "directory of that meta-repository.\n";
        std::cout << descriptionMessage << "\n" << m_optDesc << std::endl;
}

void qmeta::lib::cmd::Root::execute(std::vector<std::string> const& options) const {
      namespace po = boost::program_options;
      // Parse again...
      po::variables_map varMap;
      po::store(po::command_line_parser(options).options(m_optDesc).run(), varMap);

      boost::filesystem::path const gitRootPath = qmeta::lib::util::getRootGitDir();

      if (varMap.count("relative")) {
        std::cout << boost::filesystem::relative(
                         boost::filesystem::current_path(), gitRootPath)
                         .string()
                  << std::endl;
      } else {
        std::cout << gitRootPath.string() << std::endl;
      }
}


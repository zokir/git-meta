#ifndef LIB_CORE_ROOT_HPP
#define LIB_CORE_ROOT_HPP

#include <lib/cmd/command.hpp>

#include <boost/program_options.hpp>

#include <exception>
#include <string>
#include <vector>

namespace qmeta {
namespace lib {
namespace cmd {

class Root : Command {
    public:
        Root();
        void printHelp() const override;
        void execute(std::vector<std::string> const& options) const override;
    private:
      boost::program_options::options_description m_optDesc;
};

} // cose namespace cmd
} // close namespace lib
} // close namespace qmeta

#endif // LIB_CORE_ROOT_HPP

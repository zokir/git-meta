#ifndef LIB_CORE_COMMAND_HPP
#define LIB_CORE_COMMAND_HPP

#include <exception>
#include <string>
#include <vector>

namespace qmeta {
namespace lib {
namespace cmd {

class Command {
    public:
        Command(std::string const& summary) : m_summary(summary) {}
        virtual ~Command() = default;

        virtual void printHelp() const = 0;
        virtual void execute(std::vector<std::string> const& options) const = 0;

        std::string const& getSummary() const {
            return m_summary;
        }
    protected:
        std::string m_summary;
};

} // cose namespace core
} // close namespace lib
} // close namespace qmeta

#endif // LIB_CORE_COMMAND_HPP

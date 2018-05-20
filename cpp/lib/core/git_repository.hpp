#ifndef LIB_CORE_GIT_REPOSITORY_HPP
#define LIB_CORE_GIT_REPOSITORY_HPP

#include <git2/repository.h>
#include <string>

namespace qmeta {
namespace lib {
namespace core {

class GitRepository {
public:
    GitRepository(std::string const& path);
    ~GitRepository();

    std::string const& getPath();
private:
    git_repository *m_repository;
    std::string m_path;
};

} // cose namespace core
} // close namespace lib
} // close namespace qmeta

#endif // LIB_CORE_GIT_REPOSITORY_HPP

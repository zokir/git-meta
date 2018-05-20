#include <lib/core/git_repository.hpp>

#include <git2.h>

#include <mutex>

namespace {
std::once_flag initialized_git;

/*
void check_error(int error_code, const char *action)
{
  const git_error *error = giterr_last();
  if (!error_code)
    return;

  printf("Error %d %s - %s\n", error_code, action,
      (error && error->message) ? error->message : "???");

  exit(1);
}
TODO: C++ style check needed
*/

} // close namespace<anonymous>

qmeta::lib::core::GitRepository::GitRepository(std::string const& path):m_path(path) {
    std::call_once(initialized_git, git_libgit2_init);
	git_repository_open(&m_repository, m_path.c_str());
}

qmeta::lib::core::GitRepository::~GitRepository() {
	git_repository_free(m_repository);
}

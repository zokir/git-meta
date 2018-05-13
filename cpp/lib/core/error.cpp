#include <lib/core/error.h>

char const* qmeta::lib::core::NotInGitRepoError::what() const noexcept {
    return "Not in git repo.";
}

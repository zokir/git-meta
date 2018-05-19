#include <lib/core/error.hpp>

char const* qmeta::lib::core::NotInGitRepoError::what() const noexcept {
    return "Not in git repo.";
}

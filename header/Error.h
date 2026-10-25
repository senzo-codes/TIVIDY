#include <stdexcept>
#include <string>

class AuthorityError : public std::runtime_error {
public:
    explicit AuthorityError(const std::string& message);
};

class IllegalTransitionError : public std::logic_error {
public:
    explicit IllegalTransitionError(const std::string& message);
};
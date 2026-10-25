#ifndef TIVIDY_ROLE_H
#define TIVIDY_ROLE_H

#include <string>

class Role {
public:
    Role(int level, const std::string& name);
    int getLevel() const;
    std::string getName() const;
    bool isAutomated() const;
private:
    int level;
    std::string name;
};

#endif // TIVIDY_ROLE_H

#include "Role.h"

Role::Role(int level, const std::string &name)
    : level(level), name(name)
{
}

int Role::getLevel() const
{
    return level;
}

std::string Role::getName() const
{
    return name;
}

bool Role::isAutomated() const
{
    return level == 0;
}

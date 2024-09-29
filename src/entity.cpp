#include "entity.h"

using namespace simu;

Entity::Entity(const std::string type) : m_type(type){};

Entity::~Entity()
{
    std::cout << "Destroyed " << *this << std::endl;
}

std::string const Entity::toString()
{
    return "{type:" + m_type + ",id:" + std::to_string(m_id) + "}";
}

std::ostream& simu::operator<<(std::ostream& os, Entity& entity)
{
    std::string str = entity.toString();
    os << str;
    return os;
}
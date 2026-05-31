//
// Created by Balázs Takács on 2026. 05. 31..
//

#ifndef CMAKESFMLPROJECT_ENTITYMANAGER_H
#define CMAKESFMLPROJECT_ENTITYMANAGER_H
#include <map>

#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;


class EntityManager {
    EntityVec m_entities;
    EntityVec m_entitiesToAdd;
    EntityMap m_entityMap;
    size_t m_totalEntities = 0;

    void removeDeadEntities(EntityVec & vec);

public:
    EntityManager();

    void update();

    std::shared_ptr<Entity> addEntity(const std::string & tag);

    const EntityVec & getEntities();
    const EntityVec & getEntities(const std::string & tag);
};


#endif //CMAKESFMLPROJECT_ENTITYMANAGER_H

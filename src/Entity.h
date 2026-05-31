//
// Created by Balázs Takács on 2026. 05. 30..
//

#ifndef CMAKESFMLPROJECT_ENTITY_H
#define CMAKESFMLPROJECT_ENTITY_H
#include <string>

#include "Components.h"


class Entity {
    friend class EntityManager;

    bool m_active = true;
    size_t m_id = 0;
    std::string m_tag = "default";

    Entity(size_t id, std::string  tag);

public:
    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CShape> cShape;
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CScore> cScore;
    std::shared_ptr<CLifespan> cLifespan;

    [[nodiscard]] bool isActive() const;
    [[nodiscard]] const std::string & tag() const;
    [[nodiscard]] const size_t & id() const;
    void destroy();
};


#endif //CMAKESFMLPROJECT_ENTITY_H

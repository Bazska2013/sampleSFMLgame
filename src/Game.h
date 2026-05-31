//
// Created by Balázs Takács on 2026. 05. 30..
//

#ifndef CMAKESFMLPROJECT_GAME_H
#define CMAKESFMLPROJECT_GAME_H
#include "Entity.h"
#include "EntityManager.h"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };


class Game {
    sf::RenderWindow m_window;
    EntityManager m_entities;
    sf::Font m_font;
    sf::Text m_text;
    PlayerConfig m_playerConfig{};
    EnemyConfig m_enemyConfig{};
    BulletConfig m_bulletConfig{};
    int m_score = 0;
    int m_currentFrame = 0;
    int m_lastEnemySpawnTime = 0;
    bool m_paused = false;
    bool m_running = true;

    std::shared_ptr<Entity> m_player;

    void init(const std::string & config);
    void setPaused(bool paused);

    void sMovement();
    void sUserInput();
    void sLifespan();
    void sRender();
    void sEnemySpawner();
    void sCollision();

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnBullet(const std::shared_ptr<Entity>& entity, const Vec2 & mousePos);
    void spawnSpecialWeapon(const std::shared_ptr<Entity>& entity);

public:
    explicit Game(const std::string & config);

    void run();
};


#endif //CMAKESFMLPROJECT_GAME_H

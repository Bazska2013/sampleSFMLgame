//
// Created by Balázs Takács on 2026. 05. 30..
//

#include "Game.h"
#include <fstream>
#include <iostream>
#include <random>

Game::Game(const std::string & config) : m_text(m_font) {
    init(config);
}

void Game::init(const std::string & path) {
    int windowWidth = 1280, windowHeight = 720, frameLimit = 60, fullScreen = 0;

    std::ifstream fin(path);
    std::string token;
    while (fin >> token) {
        if (token == "Window") {
            fin >> windowWidth >> windowHeight >> frameLimit >> fullScreen;
        } else if (token == "Font") {
            std::string fontPath;
            fin >> fontPath;
            if (!m_font.openFromFile(fontPath)) {
                std::cerr << "Error loading font from " << fontPath << std::endl;
            }
            m_text.setFont(m_font);
            int charSize;
            fin >> charSize;
            m_text.setCharacterSize(charSize);
            int fontR, fontG, fontB;
            fin >> fontR >> fontG >> fontB;
            m_text.setFillColor(sf::Color(fontR, fontG, fontB));
        } else if (token == "Player") {
            fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG
                >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB
                >> m_playerConfig.OT >> m_playerConfig.V;
        } else if (token == "Enemy") {
            fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG
                >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX
                >> m_enemyConfig.L >> m_enemyConfig.SI;
        } else if (token == "Bullet") {
            fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
        }
    }

    m_window.create(sf::VideoMode({static_cast<unsigned>(windowWidth), static_cast<unsigned>(windowHeight)}), "SFML Game", fullScreen ? sf::State::Fullscreen : sf::State::Windowed);
    m_window.setFramerateLimit(frameLimit);

    spawnPlayer();
}

void Game::run() {

    while (m_running) {
        m_entities.update();
        if (!m_paused) {
            sEnemySpawner();
            sMovement();
            sLifespan();
            sCollision();
            m_currentFrame++;
        }
        sUserInput();
        sRender();
    }
}

void Game::setPaused(const bool paused) {
    m_paused = paused;
}

// respawn the player in the middle of the screen
void Game::spawnPlayer() {
    auto entity = m_entities.addEntity("player");

    // Give this entity a Transform so it spawns at (200, 200) with velocity (1, 1) and angle 0
    entity->cTransform = std::make_shared<CTransform>(Vec2(600.0f, 400.0f), Vec2(0, 0), 0.0f);

    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);

    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    entity->cInput = std::make_shared<CInput>();

    entity->cScore = std::make_shared<CScore>(0);

    m_player = entity;
}

void Game::spawnEnemy() {
    m_lastEnemySpawnTime = m_currentFrame;
    auto entity = m_entities.addEntity("enemy");
    const auto windowHeight = m_window.getSize().y;
    const auto windowWidth = m_window.getSize().x;
    entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
    entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, getRandomFromRange(m_enemyConfig.VMIN, m_enemyConfig.VMAX), sf::Color(255, 255, 255, 0), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);
    entity->cTransform = std::make_shared<CTransform>(Vec2(getRandomFromRange(0+entity->cCollision->radius, windowWidth-entity->cCollision->radius), getRandomFromRange(0+entity->cCollision->radius, windowHeight-entity->cCollision->radius)), Vec2(getRandomTrueOrFalse() ? 1.0f : -1.0f, getRandomTrueOrFalse() ? 1.0f : -1.0f), 0.0f);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
    const auto vertices = e->cShape->circle.getPointCount();
    const float angleInRadians = 360/vertices * M_PI / 180.f;

    for (size_t i = 0; i < vertices; i++) {
        auto entity = m_entities.addEntity("small_enemy");
        entity->cCollision = std::make_shared<CCollision>(e->cCollision->radius / 2);
        entity->cShape = std::make_shared<CShape>(e->cShape->circle.getRadius() / 2, e->cShape->circle.getPointCount(), e->cShape->circle.getFillColor(), e->cShape->circle.getOutlineColor(), e->cShape->circle.getOutlineThickness());
        entity->cTransform = std::make_shared<CTransform>(e->cTransform->pos, e->cTransform->pos.getVectorFromAngle(angleInRadians * i), 0.0f);
        entity->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    }
}

// spawns a bullet from a given entry to a target location
void Game::spawnBullet(const std::shared_ptr<Entity>& entity, const Vec2 & target) {

    Vec2 targetVel = (target - entity->cTransform->pos);
    targetVel.normalize();

    auto bullet = m_entities.addEntity("bullet");
    bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, Vec2(targetVel.x * m_bulletConfig.S, targetVel.y * m_bulletConfig.S), 0);
    bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
    bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
    bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
}

void Game::spawnSpecialWeapon(const std::shared_ptr<Entity>& entity) {

}

void Game::sMovement() {

    m_player->cTransform->velocity.x = 0;
    m_player->cTransform->velocity.y = 0;



    if (m_player->cInput->up) {
        m_player->cTransform->velocity.y = -m_playerConfig.S;
    }

    if (m_player->cInput->left) {
        m_player->cTransform->velocity.x = -m_playerConfig.S;
    }

    if (m_player->cInput->down) {
        m_player->cTransform->velocity.y = m_playerConfig.S;
    }

    if (m_player->cInput->right) {
        m_player->cTransform->velocity.x = m_playerConfig.S;
    }

    m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

    const auto windowHeight = m_window.getSize().y;
    const auto windowWidth = m_window.getSize().x;

    for (auto e : m_entities.getEntities("enemy")) {
        if (e->cTransform->pos.x - e->cCollision->radius < 0 || e->cTransform->pos.x + e->cCollision->radius > windowWidth) {
            e->cTransform->velocity.x *= -1;
        }
        if (e->cTransform->pos.y - e->cCollision->radius < 0 || e->cTransform->pos.y + e->cCollision->radius > windowHeight) {
            e->cTransform->velocity.y *= -1;
        }
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }

    for (auto e : m_entities.getEntities("small_enemy")) {
        if (e->cTransform->pos.x - e->cCollision->radius < 0 || e->cTransform->pos.x + e->cCollision->radius > windowWidth) {
            e->cTransform->velocity.x *= -1;
        }
        if (e->cTransform->pos.y - e->cCollision->radius < 0 || e->cTransform->pos.y + e->cCollision->radius > windowHeight) {
            e->cTransform->velocity.y *= -1;
        }
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }

    for (auto e : m_entities.getEntities("bullet")) {
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }
}

void Game::sLifespan() {

    for (auto e : m_entities.getEntities()) {
        if (e->cLifespan) {
            if (e->cLifespan->remaining > 0) {
                e->cLifespan->remaining--;
                if (e->tag() == "small_enemy") {
                    const float alphaPercentage = static_cast<float>(e->cLifespan->remaining) / static_cast<float>(e->cLifespan->total);
                    const auto fillColor = e->cShape->circle.getFillColor();
                    const auto outlineColor = e->cShape->circle.getOutlineColor();

                    int fillAlphaInt = static_cast<int>(fillColor.a * alphaPercentage + 0.5f);
                    int outlineAlphaInt = static_cast<int>(outlineColor.a * alphaPercentage + 0.5f);

                    constexpr int minAlphaInt = static_cast<int>(0.2f * 255.0f + 0.5f);

                    if (fillAlphaInt < minAlphaInt) fillAlphaInt = minAlphaInt;
                    if (outlineAlphaInt < minAlphaInt) outlineAlphaInt = minAlphaInt;

                    if (fillAlphaInt > 255) fillAlphaInt = 255;
                    if (outlineAlphaInt > 255) outlineAlphaInt = 255;

                    e->cShape->circle.setFillColor(sf::Color(fillColor.r, fillColor.g, fillColor.b, fillAlphaInt));
                    e->cShape->circle.setOutlineColor(sf::Color(outlineColor.r, outlineColor.g, outlineColor.b, outlineAlphaInt));
                }
            } else {
                e->destroy();
            }
        }
    }
}

void Game::sCollision() {
    auto bullets = m_entities.getEntities("bullet");
    auto enemies = m_entities.getEntities("enemy");
    auto small_enemies = m_entities.getEntities("small_enemy");
    for (auto e : enemies) {
        for (auto b : bullets) {
            const float dist = b->cTransform->pos.dist(e->cTransform->pos);
            if (dist < e->cCollision->radius + b->cCollision->radius) {
                spawnSmallEnemies(e);
                m_player->cScore->score += 1;
                e->destroy();
                b->destroy();
            }
        }
        const float dist = e->cTransform->pos.dist(m_player->cTransform->pos);
        if (dist < e->cCollision->radius + m_player->cCollision->radius) {
            e->destroy();
            m_player->cTransform->pos = Vec2(600.0f, 400.0f);
            m_player->cScore->score = 0;
        }
    }
    for (auto e : small_enemies) {
        for (auto b : bullets) {
            const float dist = b->cTransform->pos.dist(e->cTransform->pos);
            if (dist < e->cCollision->radius + b->cCollision->radius) {
                m_player->cScore->score += 2;
                e->destroy();
                b->destroy();
            }
        }
        const float dist = e->cTransform->pos.dist(m_player->cTransform->pos);
        if (dist < e->cCollision->radius + m_player->cCollision->radius) {
            e->destroy();
            m_player->cTransform->pos = Vec2(600.0f, 400.0f);
            m_player->cScore->score = 0;
        }
    }
}

void Game::sEnemySpawner() {
    if (m_currentFrame - m_lastEnemySpawnTime >= m_enemyConfig.SI) {
        spawnEnemy();
    }
    // (use m_currentFrame - m_lastEnemySpawnTime) to determine how long it has been since the last enemy spawned

}

void Game::sRender() {
    m_window.clear();

    m_player->cShape->circle.setPosition({m_player->cTransform->pos.x, m_player->cTransform->pos.y});
    const sf::Angle a1 = sf::radians(m_player->cTransform->angle);
    m_player->cShape->circle.setRotation(a1);

    for (auto e : m_entities.getEntities("enemy")) {
        e->cShape->circle.setPosition({e->cTransform->pos.x, e->cTransform->pos.y});
        m_window.draw(e->cShape->circle);
    }

    for (auto e : m_entities.getEntities("small_enemy")) {
        e->cShape->circle.setPosition({e->cTransform->pos.x, e->cTransform->pos.y});
        m_window.draw(e->cShape->circle);
    }

    for (auto e : m_entities.getEntities("bullet")) {
        e->cShape->circle.setPosition({e->cTransform->pos.x, e->cTransform->pos.y});
        m_window.draw(e->cShape->circle);
    }

    m_text.setString("Score: " + std::to_string(m_player->cScore->score));
    m_text.setPosition({10, 10});

    m_window.draw(m_text);

    m_window.draw(m_player->cShape->circle);
    m_window.display();
}

void Game::sUserInput() {

    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_running = false;
            m_window.close();
        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                m_running = false;
                m_window.close();
            }
            if (!m_paused) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::W) {
                    m_player->cInput->up = true;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::A) {
                    m_player->cInput->left = true;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::S) {
                    m_player->cInput->down = true;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::D) {
                    m_player->cInput->right = true;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::P) {
                    setPaused(true);
                }
            } else {
                if (keyPressed->scancode == sf::Keyboard::Scancode::P) {
                    setPaused(false);
                }
            }
        } else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            if (keyReleased->scancode == sf::Keyboard::Scancode::W) {
                m_player->cInput->up = false;
            }
            if (keyReleased->scancode == sf::Keyboard::Scancode::A) {
                m_player->cInput->left = false;
            }
            if (keyReleased->scancode == sf::Keyboard::Scancode::S) {
                m_player->cInput->down = false;
            }
            if (keyReleased->scancode == sf::Keyboard::Scancode::D) {
                m_player->cInput->right = false;
            }
        } else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (!m_paused) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                    spawnBullet(m_player, Vec2(mouseButtonPressed->position.x, mouseButtonPressed->position.y));
                }
            }
        }
    }

    if (!m_paused) {
        sf::Vector2i localPosition = sf::Mouse::getPosition(m_window);
        m_player->cTransform->angle = m_player->cTransform->pos.angle(Vec2(localPosition.x, localPosition.y));
    }
}

double Game::getRandomFromRange(const int min, const int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(min, max);
    return distrib(gen);
}

bool Game::getRandomTrueOrFalse() {
    return getRandomFromRange(0, 1) < 0.5;
}

#include<units/enemy.hpp>

#include<items/food.hpp>
#include<items/weapon.hpp>
#include<items/ammo.hpp>
#include<items/armor.hpp>
#include<direction.hpp>
#include<units/hero.hpp>
#include<game.hpp>

#include<effolkronium/random.hpp>

#include<fmt/format.h>

#include<queue>

using Random = effolkronium::random_static;

Enemy::Enemy(Enemy const & other)
    : Unit(other)
    , target(other.target)
    , lastTurnMoved(other.lastTurnMoved)
    , xpCost(other.xpCost) {
    if (other.ammo == nullptr) {
        ammo = nullptr;
    } else {
        ammo = dynamic_cast<Ammo *>(&inventory[other.ammo->inventorySymbol]);
    }
}

Enemy & Enemy::operator =(Enemy const & other) {
    if (this == &other) {
        return *this;
    }
    Unit::operator =(other);
    target = other.target;
    lastTurnMoved = other.lastTurnMoved;
    xpCost = other.xpCost;
    if (other.ammo == nullptr) {
        ammo = nullptr;
    } else {
        ammo = dynamic_cast<Ammo *>(&inventory[other.ammo->inventorySymbol]);
    }
    return *this;
}

void Enemy::dropInventory() {
    ammo = nullptr;
    Unit::dropInventory();
}

void Enemy::shoot() {
    if (weapon == nullptr or ammo == nullptr)
        return;

    auto dir = directionFrom(g_game.getHero().pos - pos).value();
    Vec2i offset = toVec2i(dir);
    char sym = toChar(dir);
    for (int i = 1; i < weapon->range + ammo->range; i++) {
        Coord2i cell = pos + offset * i;

        if (g_game.level()[cell] == 2)
            break;

        auto const & unitMap = g_game.getUnitMap();
        if (unitMap[cell] and unitMap[cell]->getType() == Unit::Type::Hero) {
            g_game.getHero().dealDamage(ammo->damage + weapon->damageBonus);
            break;
        }
        g_game.getRenderer()
            .setCursorPosition(cell)
            .put(sym)
            .display();
        sleep(DELAY / 3);
    }

    ammo->count--;
    if (ammo->count <= 0) {
        inventory.remove(ammo->inventorySymbol);
        ammo = nullptr;
    }
}

tl::optional<Coord2i> Enemy::searchForShortestPath(Coord2i to) const {
    if (to == pos)
        return {};

    int maxDepth = 2 + std::abs(to.x - pos.x) + std::abs(to.y - pos.y);

    std::queue<Coord2i> q;
    q.push(pos);

    Array2D<int, LEVEL_ROWS, LEVEL_COLS> used;
    used[pos] = 1;

    std::vector<Vec2i> dirs = {
        toVec2i(Direction::Up),
        toVec2i(Direction::Down),
        toVec2i(Direction::Right),
        toVec2i(Direction::Left)
    };
    if (g_game.getMode() == 2) {
        dirs.push_back(toVec2i(Direction::UpRight));
        dirs.push_back(toVec2i(Direction::UpLeft));
        dirs.push_back(toVec2i(Direction::DownRight));
        dirs.push_back(toVec2i(Direction::DownLeft));
    }

    while (not q.empty()) {
        Coord2i v = q.front();
        if (v == to)
            break;

        if (used[v] > maxDepth)
            return {};

        q.pop();

        for (auto dir : dirs) {
            auto tv = v + dir;
            auto const & unitMap = g_game.getUnitMap();
            if (unitMap.isIndex(tv)
                    and (not unitMap[tv] or unitMap[tv]->getType() == Unit::Type::Hero)
                    and g_game.level()[tv] != 2 and used[tv] == 0) {
                q.push(tv);
                used[tv] = 1 + used[v];
            }
        }
    }

    if (not used[to])
        return {};

    Coord2i v = to;
    while (used[v] > 2) {
        for (auto dir : dirs) {
            auto tv = v - dir;
            if (used.isIndex(tv) and used[tv] + 1 == used[v]) {
                v = tv;
                break;
            }
        }
    }

    return v;
}

void Enemy::moveTo(Coord2i cell) {
    if (g_game.level()[cell] == 2)
        throw std::logic_error("Trying to move an enemy into a wall");

    auto const & unitMap = g_game.getUnitMap();
    if (not unitMap[cell]) {
        setTo(cell);
        return;
    }

    if (unitMap[cell]->getType() == Unit::Type::Enemy or weapon == nullptr)
        return;

    auto & hero = g_game.getHero();
    if (hero.armor == nullptr or hero.armor->mdf != 2) {
        hero.dealDamage(weapon->damage);
    } else {
        dealDamage(weapon->damage);
    }

    if (health <= 0) {
        g_game.getUnitMap().killUnit(*this);
    }
}

void Enemy::updatePosition() {
    lastTurnMoved = g_game.getTurnNumber();
    auto const & hero = g_game.getHero();

    if (not hero.isInvisible() and canSee(hero.pos)) {
        bool onDiagLine = std::abs(hero.pos.y - pos.y) == std::abs(hero.pos.x - pos.x);
        bool canShootHero = (pos.y == hero.pos.y or pos.x == hero.pos.x or onDiagLine)
                and weapon and weapon->isRanged and ammo
                and weapon->range + ammo->range >= std::abs(hero.pos.y - pos.y) + std::abs(hero.pos.x - pos.x);
        if (canShootHero) {
            shoot();
        } else {
            target = hero.pos;

            if (auto next = searchForShortestPath(hero.pos)) {
                moveTo(*next);
                return;
            }
        }
    }
    tl::optional<Coord2i> next;
    if (target.has_value() and (next = searchForShortestPath(*target))) {
        moveTo(*next);
        return;
    }

    std::vector<Coord2i> visibleCells;

    g_game.getUnitMap().forEach([&] (Coord2i cell, Ptr<Unit> const & unit) {
        if (cell != pos and g_game.level()[cell] != 2 and not unit and canSee(cell)) {
            visibleCells.push_back(cell);
        }
    });

    int attempts = 15;
    for (int i = 0; i < attempts; ++i) {
        target = *Random::get(visibleCells);

        if (auto next = searchForShortestPath(*target)) {
            moveTo(*next);
            return;
        }
    }
}


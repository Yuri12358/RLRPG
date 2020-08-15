#include<items/weapon.hpp>

#include<items/ammo.hpp>

#include<cassert>
#include<memory>

int Weapon::getShootDamage() const {
    assert(isRanged);
    assert(not cartridge.isEmpty());

    return cartridge.isEmpty() ? 0 : damageBonus + cartridge.next().damage;
}

int Weapon::getShootDistance() const {
    assert(isRanged);
    assert(not cartridge.isEmpty());

    return cartridge.isEmpty() ? 0 : range + cartridge.next().range;
}

Weapon::Cartridge::Cartridge(int capacity): capacity(capacity) {
    assert(capacity >= 0);
}

Weapon::Cartridge::Cartridge(Cartridge const & other): capacity(other.capacity) {
    for (auto const & bullet : other) {
        loaded.push_back(std::make_unique<Ammo>(*bullet));
    }
}

Weapon::Cartridge & Weapon::Cartridge::operator =(Cartridge const & other) {
    capacity = other.capacity;
    loaded.clear();
    for (auto const & bullet : other) {
        loaded.push_back(std::make_unique<Ammo>(*bullet));
    }
    return *this;
}

Ptr<Ammo> Weapon::Cartridge::load(Ptr<Ammo> bullet) {
    if (loaded.size() == capacity) {
        return bullet;
    }
    loaded.push_back(std::move(bullet));
    return nullptr;
}

Ptr<Ammo> Weapon::Cartridge::unloadOne() {
    if (loaded.empty()) {
        return nullptr;
    }
    auto bullet = std::move(loaded.back());
    loaded.pop_back();
    return bullet;
}

Ammo const * Weapon::Cartridge::operator [](int ind) const {
    assert(ind >= 0 and ind < capacity);
    if (ind < loaded.size()) {
        return loaded[ind].get();
    }
    return nullptr;
}

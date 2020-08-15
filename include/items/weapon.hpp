#ifndef RLRPG_ITEMS_WEAPON_HPP
#define RLRPG_ITEMS_WEAPON_HPP

#include<items/item.hpp>
#include<ptr.hpp>
#include<enable_clone.hpp>
#include<vector>
#include<cassert>

class Ammo;

class Weapon
    : public Item
    , public EnableClone<Weapon>
{
public:
    class Cartridge {
        std::vector<Ptr<Ammo>> loaded;
        int capacity = 0;

    public:
        explicit Cartridge(int capacity = 0);
        Cartridge(Cartridge const &);
        Cartridge & operator =(Cartridge const &);

        // returns the bullet if fails to load it
        Ptr<Ammo> load(Ptr<Ammo> bullet);

        Ptr<Ammo> unloadOne();

        Ammo& next() {
            assert(not isEmpty());
            return *loaded.back();
        }

        Ammo const& next() const {
            assert(not isEmpty());
            return *loaded.back();
        }

        Ammo const * operator [](int ind) const;

        auto begin() const {
            return loaded.begin();
        }

        auto end() const {
            return loaded.end();
        }

        int getCapacity() const {
            return capacity;
        }

        int getCurrSize() const {
            return static_cast<int>(loaded.size());
        }

        bool isEmpty() const {
            return loaded.empty();
        }

        bool isFull() const {
            return loaded.size() == capacity;
        }
    };

    static int const COUNT = 25; /* JUST FOR !DEBUG!!*/

    Cartridge cartridge;
    int damage;
    int range;                                     // Ranged bullets have additional effect on this paramether
    int damageBonus;                                // And on this too
    bool isRanged = false;
    bool canDig = false;

    Type getType() const override {
        return Type::Weapon;
    }

    Ptr<Item> cloneItem() const override {
        return std::make_unique<Weapon>(*this);
    }

    int getShootDamage() const;
    int getShootDistance() const;
};

#endif // RLRPG_ITEMS_WEAPON_HPP


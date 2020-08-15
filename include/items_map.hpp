#ifndef RLRPG_ITEMS_MAP_HPP
#define RLRPG_ITEMS_MAP_HPP

#include<level.hpp>
#include<array2d.hpp>
#include<ptr.hpp>
#include<vec2.hpp>

#include<list>
#include<string_view>

class Item;

using ItemPile = std::list<Ptr<Item>>;

class ItemsMap : public Array2D<ItemPile, LEVEL_ROWS, LEVEL_COLS> {
public:
    /*
    Requires `cell` to be a valid coord
    */
    ItemPile::iterator findItemAt(Coord2i cell, std::string_view id);

    /*
    Requires:
    - `item != nullptr`
    - `cell` is a valid coord
    */
    void drop(Ptr<Item> item, Coord2i cell);
};

#endif // RLRPG_ITEMS_MAP_HPP
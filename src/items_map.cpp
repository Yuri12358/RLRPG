#include<items_map.hpp>
#include<compat/algorithm.hpp>
#include<items/item.hpp>
#include<items/helpers.hpp>
#include<functional.hpp>

#include<cassert>

ItemPile::iterator ItemsMap::findItemAt(Coord2i cell, std::string_view id) {
    assert(isIndex(cell));

    return compat::find_if(at(cell), compose(searchByItemID(id), deref));
}

void ItemsMap::drop(Ptr<Item> item, Coord2i cell) {
    assert(item != nullptr);
    assert(isIndex(cell));

    item->pos = cell;
    if (item->isStackable) {
        if (auto const it = findItemAt(cell, item->id); it != end(at(cell))) {
            (*it)->count += item->count;
            return;
        }
    }
    at(cell).push_back(std::move(item));
}
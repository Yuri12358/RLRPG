#ifndef RLRPG_ITEMS_HELPERS_HPP
#define RLRPG_ITEMS_HELPERS_HPP

#include<items/item.hpp>

#include<tl/optional.hpp>

#include<string_view>
#include<cctype>

inline bool compareByInventoryID(Item const& lhs, Item const& rhs) noexcept {
    return lhs.inventorySymbol < rhs.inventorySymbol;
}

inline tl::optional<int> idToIndex(char id) {
    if (std::islower(id)) {
        return id - 'a';
    }
    if (std::isupper(id)) {
        return id - 'A' + 26;
    }
    return tl::nullopt;
}

inline auto searchByItemType(Item::Type type) {
    return [type] (Item const& item) {
        return item.getType() == type;
    };
}

inline auto searchByInventoryID(char id) {
    return [id] (Item const& item) {
        return item.inventorySymbol == id;
    };
}

inline auto searchByItemID(std::string_view id) {
    return [id] (Item const& item) {
        return item.id == id;
    };
}

#endif // RLRPG_ITEMS_HELPERS_HPP

#include<unit_map.hpp>
#include<units/unit.hpp>

void UnitMap::placeUnitAt(Ptr<Unit>&& unit, Coord2i cell) {
    assert(unit != nullptr);

    if (isIndex(cell) and at(cell) == nullptr) {
        unit->pos = cell;
        at(cell) = std::move(unit);
    }
}

void UnitMap::killUnit(Unit& unit) {
    unit.dropInventory();
    at(unit.pos).reset();
}

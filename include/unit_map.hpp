#ifndef RLRPG_UNIT_MAP_HPP
#define RLRPG_UNIT_MAP_HPP

#include<array2d.hpp>
#include<level.hpp>
#include<ptr.hpp>
#include<vec2.hpp>

class Unit;

class UnitMap : public Array2D<Ptr<Unit>, LEVEL_ROWS, LEVEL_COLS> {
public:
    /*
    Requires:
    - `unit != nullptr`

    If cell is invalid or occupied, the ownership over `unit` won't be taken
    */
    void placeUnitAt(Ptr<Unit>&& unit, Coord2i cell);

    /*
    Requires that `unit` is on this unitmap
    */
    void killUnit(Unit& unit);
};

#endif // RLRPG_UNIT_MAP_HPP
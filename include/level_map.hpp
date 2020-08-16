#ifndef RLRPG_LEVEL_MAP_HPP
#define RLRPG_LEVEL_MAP_HPP

#include<array2d.hpp>
#include<level.hpp>
#include<level_cell.hpp>
#include<compat/cstring_view.hpp>

#include<string_view>
#include<iosfwd>

class LevelMap : public Array2D<LevelCell, LEVEL_ROWS, LEVEL_COLS> {
public:
    void fillWith(LevelCell value) {
        forEach([value] (LevelCell& cell) {
            cell = value;
        });
    }

    bool loadFromFile(compat::cstring_view filename);
    bool loadFromStream(std::istream& input);
};

#endif // RLRPG_LEVEL_MAP_HPP
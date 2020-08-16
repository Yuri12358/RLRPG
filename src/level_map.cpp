#include<level_map.hpp>

#include<tl/optional.hpp>

#include<fstream>
#include<cassert>

namespace {
    [[nodiscard]]
    tl::optional<LevelCell> fromCode(int code) {
        switch (code) {
            case 1: return LevelCell::Empty;
            case 2: return LevelCell::Wall;
            default: return tl::nullopt;
        }
    }
}

bool LevelMap::loadFromFile(compat::cstring_view filepath) {
    std::ifstream file{ filepath.c_str() };
    if (not file.is_open()) {
        return false;
    }
    
    return loadFromStream(file);
}

bool LevelMap::loadFromStream(std::istream& input) {
    assert(input);

    bool failed = false;
    forEach([&input, &failed] (LevelCell& cell) {
        if (failed) {
            return;
        }

        int code;
        input >> code;
        if (auto maybeCell = fromCode(code)) {
            cell = *maybeCell;
        } else {
            failed = true;
        }
    });

    return failed;
}

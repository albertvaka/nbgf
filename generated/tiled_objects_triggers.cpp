#include "tiled_objects_triggers.h"

namespace Tiled {


const std::array<BoxBounds, 1> Triggers::trigger_fast_lava = {
	BoxBounds(2128.148f,1244.0f,577.705f,156.0f),
};
const BoxBounds Triggers::single_trigger_fast_lava = BoxBounds(2128.148f,1244.0f,577.705f,156.0f);


const std::array<BoxBounds, 1> Triggers::trigger_lava = {
	BoxBounds(1600.62f,1150.909f,158.705f,269.333f),
};
const BoxBounds Triggers::single_trigger_lava = BoxBounds(1600.62f,1150.909f,158.705f,269.333f);


const std::array<BoxBounds, 1> Triggers::trigger_leafs = {
	BoxBounds(2155.0f,1697.0833f,42.0f,12.1667f),
};
const BoxBounds Triggers::single_trigger_leafs = BoxBounds(2155.0f,1697.0833f,42.0f,12.1667f);


const std::array<BoxBounds, 1> Triggers::trigger_rockfall = {
	BoxBounds(6419.33f,1537.583f,33.5f,100.0f),
};
const BoxBounds Triggers::single_trigger_rockfall = BoxBounds(6419.33f,1537.583f,33.5f,100.0f);


const std::array<BoxBounds, 1> Triggers::trigger_test = {
	BoxBounds(2526.0f,1787.0f,42.0f,55.5f),
};
const BoxBounds Triggers::single_trigger_test = BoxBounds(2526.0f,1787.0f,42.0f,55.5f);



}
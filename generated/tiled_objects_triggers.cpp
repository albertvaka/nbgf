#include "tiled_objects_triggers.h"

namespace Tiled {


const std::array<BoxBounds, 1> Triggers::trigger_fast_lava = {
	BoxBounds(2128.1476666666667f,1244.0001666666672f,577.704666666667f,155.999666666667f),
};
const BoxBounds Triggers::single_trigger_fast_lava = BoxBounds(2128.1476666666667f,1244.0001666666672f,577.704666666667f,155.999666666667f);


const std::array<BoxBounds, 1> Triggers::trigger_lava = {
	BoxBounds(1600.61736363636f,1150.909257575757f,158.704666666667f,269.333f),
};
const BoxBounds Triggers::single_trigger_lava = BoxBounds(1600.61736363636f,1150.909257575757f,158.704666666667f,269.333f);


const std::array<BoxBounds, 1> Triggers::trigger_rockfall = {
	BoxBounds(5207.33333333333f,1537.583333333333f,33.5f,100.0f),
};
const BoxBounds Triggers::single_trigger_rockfall = BoxBounds(5207.33333333333f,1537.583333333333f,33.5f,100.0f);


const std::array<BoxBounds, 1> Triggers::trigger_test = {
	BoxBounds(2526.0f,1787.0f,42.0f,55.5f),
};
const BoxBounds Triggers::single_trigger_test = BoxBounds(2526.0f,1787.0f,42.0f,55.5f);



}
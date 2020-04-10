import pytiled_parser
from jinja2 import Template
from pathlib import Path
from  collections import defaultdict

level = pytiled_parser.parse_tile_map("level.tmx")
tileset = None
tileset_cols = 0
tilesize = 0
for ts in level.tile_sets.values():
    if ts.name == 'mario':
        tileset = ts.tiles
        tileset_cols = ts.columns
        tilesize = ts.max_tile_size.height
        break

tiles = None
entities = None
for l in level.layers:
    if l.name == "World":
        tiles = l
    elif l.name == "Entities":
        entities = l

out_map_dict = dict()
gids_used = set()

min_x = 0
min_y = 0
max_x = 0
max_y = 0

for chunk in tiles.data:
    if chunk.location.x < min_x:
        min_x = chunk.location.x 
    elif chunk.location.x + chunk.width > max_x:
        max_x = chunk.location.x + chunk.width
    if chunk.location.y < min_y:
        min_y = chunk.location.y
    elif chunk.location.y + chunk.height > max_y:
        max_y = chunk.location.y + chunk.height

out_width = max_x - min_x
out_height = max_y - min_y

for chunk in tiles.data:
    y = chunk.location.y - min_y
    for row in chunk.chunk_data:
        x = chunk.location.x - min_x
        for t in row:
            t-=1 #so index matches the tileset ids, if it was 0 it meant no tile
            pos = x + y*out_width
            if out_map_dict.get(pos):
                print("BUG")
            gids_used.add(t)
            out_map_dict[pos] = t
            x+=1
        y+=1

gids_used.remove(-1)

gids_by_type = defaultdict(list)
for g in gids_used:
    type_ = tileset[g].type_ if g in tileset else "bg"
    gids_by_type[type_].append(g)

unknown_t = gids_by_type.keys() - ['bg','oneway','rslope','lslope','solid','breakable']
if len(unknown_t) > 0:
    print("Unknown types found:", unknown_t)

current_tileid = 1
gid_to_tileid = {-1:0}
for type_ in ['bg','oneway','rslope','lslope','solid','breakable']:
    for g in gids_by_type[type_]:
        gid_to_tileid[g] = current_tileid
        current_tileid += 1

out_map = [ gid_to_tileid[out_map_dict.get(i,-1)] for i in range(out_width*out_height) ]

print("Different tiles used:",len(gids_used))
print("Width: {}, height: {}, total tiles: {}".format(out_width, out_height, len(out_map)))
print("Tile types founds:", ','.join(gids_by_type.keys()))

tm = Template(Path('tiledexport.h.tmpl').read_text())
out_h = tm.render(
    bg=gids_by_type['bg'],
    oneway=gids_by_type['oneway'],
    rslope=gids_by_type['rslope'],
    lslope=gids_by_type['lslope'],
    solid=gids_by_type['solid'],
    breakable=gids_by_type['breakable'],
)
Path('../src/tiledexport.h').write_text(out_h)
                
tm = Template(Path('tiledexport.cpp.tmpl').read_text())
#template.globals['custom_function'] = custom_function
out_cpp = tm.render(
    bg=gids_by_type['bg'],
    oneway=gids_by_type['oneway'],
    rslope=gids_by_type['rslope'],
    lslope=gids_by_type['lslope'],
    solid=gids_by_type['solid'],
    breakable=gids_by_type['breakable'],
    tilesize = tilesize,
    tileset_cols = tileset_cols,
    width = out_width,
    height = out_height,
    gid_to_tileid= gid_to_tileid,
    map = out_map,
    debug = False
)
Path('../src/tiledexport.cpp').write_text(out_cpp)

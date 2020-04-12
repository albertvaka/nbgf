import pytiled_parser
from jinja2 import Template
from pathlib import Path
from collections import defaultdict
from itertools import chain

known_types = ['bg','oneway','rslope','lslope','solid','breakable']

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
areas = None
screens = None
for l in level.layers:
    if l.name == "World":
        tiles = l
    elif l.name == "Entities":
        entities = l.tiled_objects
    elif l.name == "Areas":
        areas = l.tiled_objects
    elif l.name == "Screens":
        screens = l.tiled_objects

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

unknown_t = gids_by_type.keys() - known_types
if len(unknown_t) > 0:
    print("Unknown types found:", unknown_t)

current_tileid = 1
gid_to_tileid = {-1:0}
aliases = dict()
tilenames = {}
for type_ in known_types:
    num = 1
    for g in gids_by_type[type_]:
        gid_to_tileid[g] = current_tileid
        name = '{}_{}'.format(type_.upper(), num).upper()
        tilenames[g] = name
        current_tileid += 1
        num +=1
        if g in tileset and tileset[g].properties is not None:
            for p in tileset[g].properties:
                if p.name == 'alias':
                    aliases[name] = '{}_{}'.format(type_.upper(), p.value).upper()  

out_map = [ gid_to_tileid[out_map_dict.get(i,-1)] for i in range(out_width*out_height) ]

print("Different tiles used:",len(gids_used))
print("Width: {}, height: {}, total tiles: {}".format(out_width, out_height, len(out_map)))
print("Tile types founds:", ','.join(gids_by_type.keys()))

all_gids = []
for t in known_types:
    all_gids += gids_by_type[t]

entities_by_type = defaultdict(list)
for e in entities:
    type_ = e.type if e.type != None else tileset[e.gid-1].type_
    entities_by_type[type_].append((e.location.x-min_x*tilesize, e.location.y-min_y*tilesize))

areas_by_type = defaultdict(list)
for e in areas:
    areas_by_type[e.type].append((e.location.x-min_x*tilesize, e.location.y-min_y*tilesize, e.size.width, e.size.height))

out_screens = []
for e in screens:
    out_screens.append((e.location.x-min_x*tilesize, e.location.y-min_y*tilesize,e.size.width,e.size.height))

tm = Template(Path('tiledexport.h.tmpl').read_text())
out_h = tm.render(
    gids=all_gids,
    tilenames=tilenames,
    aliases=aliases,
    entities_by_type=entities_by_type,
    areas_by_type=areas_by_type,
    screens=out_screens,
)

current = Path('../src/tiledexport.h').read_text()
if out_h == current:
    print("Skipping header generation, no changes detected")
else:
    Path('../src/tiledexport.h').write_text(out_h)
                
tm = Template(Path('tiledexport.cpp.tmpl').read_text())
#template.globals['custom_function'] = custom_function
out_cpp = tm.render(
    gids = all_gids,
    tilenames = tilenames,
    tilesize = tilesize,
    tileset_cols = tileset_cols,
    width = out_width,
    height = out_height,
    gid_to_tileid= gid_to_tileid,
    map = out_map,
    entities_by_type=entities_by_type,
    areas_by_type=areas_by_type,
    screens=out_screens,
    debug = False,
)

current = Path('../src/tiledexport.cpp').read_text()
if out_cpp == current:
    print("Skipping cpp generation, no changes detected")
else:
    Path('../src/tiledexport.cpp').write_text(out_cpp)

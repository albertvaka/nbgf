import pytiled_parser
from jinja2 import Template
from pathlib import Path
import os
from collections import defaultdict, OrderedDict

level = pytiled_parser.parse_map(Path("tiled.tmx"))

known_tile_types = ['bg','oneway','rslope','lslope','solid','breakable', 'breakable_hard', 'breakable_gnd']

# Tilesets
tilesets = list(level.tilesets.values())
if len(tilesets) > 1:
    print("Warning: Found {} tilesets. Only the first tileset will be used".format(len(tilesets)))
ts = tilesets[0]
tileset = ts.tiles
tileset_cols = ts.columns
tilesize = ts.tile_width

# Layers
tile_layers = []
object_layers = []
for l in level.layers:
    if l.properties is not None and l.properties.get('noexport',False):
        continue
    if isinstance(l, pytiled_parser.ObjectLayer):
        object_layers.append(l)
    elif isinstance(l, pytiled_parser.TileLayer):
        tile_layers.append(l)
    else:
        print("Ignoring layer with unknown type "+l.name)


# Tile layers

if len(tile_layers) > 1:
    print("Warning: Found {} tile layers. Only the first one will be exported".format(len(tile_layers)))

all_gids = []
gids_used = set()
current_tileid = 1
gid_to_tileid = {-1:0}
aliases = dict()
tilenames = {}

tiles = tile_layers[0]
print("Exporting tile layer '{}'".format(tiles.name))
out_map_dict = dict()

min_x = 0
min_y = 0
max_x = 0
max_y = 0

for chunk in tiles.chunks:
    if chunk.coordinates.x < min_x:
        min_x = chunk.coordinates.x
    elif chunk.coordinates.x + chunk.size.width > max_x:
        max_x = chunk.coordinates.x + chunk.size.width
    if chunk.coordinates.y < min_y:
        min_y = chunk.coordinates.y
    elif chunk.coordinates.y + chunk.size.height > max_y:
        max_y = chunk.coordinates.y + chunk.size.height

out_width = max_x - min_x
out_height = max_y - min_y

for chunk in tiles.chunks:
    y = chunk.coordinates.y - min_y
    for row in chunk.data:
        x = chunk.coordinates.x - min_x
        for t in row:
            t-=1 #so index matches the tileset ids, if it was 0 it meant no tile
            pos = x + y*out_width
            if out_map_dict.get(pos):
                print("BUG")
            gids_used.add(t)
            out_map_dict[pos] = t
            x+=1
        y+=1

# Mark tiles with aliases as used so they appear in the
# exported file, since they might be used programatically
for g in tileset.keys():
    if tileset[g].properties is not None:
        for p in tileset[g].properties:
            if p == 'alias':
                gids_used.add(g)

gids_used.remove(-1)

gids_used = sorted(gids_used)

gids_by_type = defaultdict(list)
for g in gids_used:
    type_ = tileset[g].class_ if g in tileset and tileset[g].class_ is not None else "bg"
    gids_by_type[type_].append(g)


print("Found these tile types:",  ', '.join(gids_by_type.keys()))

ordered_tiletypes = OrderedDict()
for a in known_tile_types+list(gids_by_type.keys()):
    if a in gids_by_type:
        ordered_tiletypes[a] = 1 # We use it as a set

print("Which will be ordered as:",  ', '.join(ordered_tiletypes))

for type_ in ordered_tiletypes:
    num = 1
    for g in gids_by_type[type_]:
        gid_to_tileid[g] = current_tileid
        current_tileid += 1
        name = '{}_{}'.format(type_.upper(), num).upper()
        num += 1
        tilenames[g] = name
        if g in tileset and tileset[g].properties is not None:
            for p, v in tileset[g].properties.items():
                if p == 'alias':
                    for alias in v.split(";"):
                        full_alias = '{}_{}'.format(type_, alias).upper()
                        aliases[full_alias] = name

out_map = [ gid_to_tileid[out_map_dict.get(i,-1)] for i in range(out_width*out_height) ]

print("Width: {}, height: {}, total tiles: {}".format(out_width, out_height, len(out_map)))

for type_ in ordered_tiletypes:
    all_gids += gids_by_type[type_]

tm = Template(Path('tiled_tilemap.h.tmpl').read_text())
out_content = tm.render(
    name=tiles.name,
)

fname = '../generated/tiled_tilemap.h'
try:
    current = Path(fname).read_text()
except FileNotFoundError:
    current = ""
if out_content == current:
    print("NO CHANGES: {}".format(os.path.basename(fname)))
else:
    print("UPDATING: {}".format(os.path.basename(fname)))
    Path(fname).write_text(out_content)


tm = Template(Path('tiled_tilemap.cpp.tmpl').read_text())
#template.globals['custom_function'] = custom_function
out_content = tm.render(
    name=tiles.name,
    width = out_width,
    height = out_height,
    map = out_map,
    debug = False,
)

fname = '../generated/tiled_tilemap.cpp'
try:
    current = Path(fname).read_text()
except FileNotFoundError:
    current = ""
if out_content == current:
    print("NO CHANGES: {}".format(os.path.basename(fname)))
else:
    print("UPDATING: {}".format(os.path.basename(fname)))
    Path(fname).write_text(out_content)



# Object layers

for objects in object_layers:
    print("Exporting objects layer '{}'".format(objects.name))

    entities_by_type = defaultdict(list)
    areas_by_type = defaultdict(list)
    types_with_rotation = set()

    for e in objects.tiled_objects:
        if isinstance(e,  pytiled_parser.tiled_object.Tile):
            type_ = e.class_ if e.class_ else tileset[e.gid-1].class_
            if type_:
                entities_by_type[type_].append((e.id, float(e.coordinates.x)-min_x*tilesize, float(e.coordinates.y)-min_y*tilesize, e.rotation))
                if e.rotation != 0:
                    types_with_rotation.add(type_)
            else:
                print("Warning: ignoring object without type nor tile type")
        elif isinstance(e, pytiled_parser.tiled_object.Rectangle):
            if e.class_:
                areas_by_type[e.class_].append((float(e.coordinates.x)-min_x*tilesize, float(e.coordinates.y)-min_y*tilesize, float(e.size.width), float(e.size.height)))
            else:
                print("Warning: ignoring rectangle without type")

    transforms_by_type = defaultdict(list)
    for t in types_with_rotation:
        transforms_by_type[t] = entities_by_type.pop(t)

    tm = Template(Path('tiled_objects.h.tmpl').read_text())
    out_content = tm.render(
        name=objects.name,
        entities_by_type=sorted(entities_by_type.items()),
        transforms_by_type=sorted(transforms_by_type.items()),
        areas_by_type=sorted(areas_by_type.items()),
    )

    fname = '../generated/tiled_objects_{}.h'.format(objects.name.lower())
    try:
        current = Path(fname).read_text()
    except FileNotFoundError:
        current = ""
    if out_content == current:
        print("NO CHANGES: {}".format(os.path.basename(fname)))
    else:
        print("UPDATING: {}".format(os.path.basename(fname)))
        Path(fname).write_text(out_content)

    tm = Template(Path('tiled_objects.cpp.tmpl').read_text())
    #template.globals['custom_function'] = custom_function
    out_content = tm.render(
        name=objects.name,
        entities_by_type=sorted(entities_by_type.items()),
        transforms_by_type=sorted(transforms_by_type.items()),
        areas_by_type=sorted(areas_by_type.items()),
        debug = False,
    )

    fname = '../generated/tiled_objects_{}.cpp'.format(objects.name.lower())
    try:
        current = Path(fname).read_text()
    except FileNotFoundError:
        current = ""
    if out_content == current:
        print("NO CHANGES: {}".format(os.path.basename(fname)))
    else:
        print("UPDATING: {}".format(os.path.basename(fname)))
        Path(fname).write_text(out_content)



print("Total different tiles used:", len(gids_used))
# We use uint16 now
#if len(gids_used) > 255:
#    print("Warning: There are more than 255 tiles used, but we try to represent them as a uint8!")

# Tileset
tm = Template(Path('tiled_tile.h.tmpl').read_text())
out_content = tm.render(
    name=ts.name,
    gids=all_gids,
    tilenames=tilenames,
    tilesize = tilesize,
    aliases=aliases,
)

fname = '../generated/tiled_tile.h'
try:
    current = Path(fname).read_text()
except FileNotFoundError:
    current = ""
if out_content == current:
    print("NO CHANGES: {}".format(os.path.basename(fname)))
else:
    print("UPDATING: {}".format(os.path.basename(fname)))
    Path(fname).write_text(out_content)

tm = Template(Path('tiled_tile.cpp.tmpl').read_text())
#template.globals['custom_function'] = custom_function
out_content = tm.render(
    name=ts.name,
    gids = all_gids,
    tilenames = tilenames,
    tilesize = tilesize,
    tileset_cols = tileset_cols,
    gid_to_tileid= gid_to_tileid,
    debug = False,
)

fname = '../generated/tiled_tile.cpp'
try:
    current = Path(fname).read_text()
except FileNotFoundError:
    current = ""
if out_content == current:
    print("NO CHANGES: {}".format(os.path.basename(fname)))
else:
    print("UPDATING: {}".format(os.path.basename(fname)))
    Path(fname).write_text(out_content)

# VS compares the dates from this and tiled.tmx to know if it needs to re-trigger this script.
# We can't use the dates from the generated files themselves because we don't change the files
# if there are no changes to not trigger a rebuild.
Path("../obj/last_tiled_generation").touch()

import json
from jinja2 import Template
from pathlib import Path
from collections import defaultdict

rects = {}

import glob, os
for file in glob.glob("*.json"):

    with open(file) as json_file:
        data = json.load(json_file)
        for frame in data['frames']:
            name = frame['filename'].upper().replace('-','_').replace(' ','_').replace('/','_').replace('.PNG','')
            rect = frame['frame']
            rects[name] = (rect['x'], rect['y'], rect['w'], rect['h'])        


tm = Template(Path('texturepacker.h.tmpl').read_text())
out_h = tm.render(
    rects=rects,
)

try:
    current = Path('../src/texturepackerexport.h').read_text()
except FileNotFoundError:
    current = ""
if out_h == current:
    print("Skipping header generation, no changes detected")
else:
    Path('../src/texturepackerexport.h').write_text(out_h)


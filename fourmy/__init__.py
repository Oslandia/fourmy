# coding = utf-8

import os
import ctypes

dll_file = None
for f in os.listdir(os.path.dirname(__file__)):
    if f[:7] == '_fourmy' and (f.endswith(".so") or f.endswith(".pyd")):
        dll_file = os.path.join(os.path.dirname(__file__), f)

_fourmy = ctypes.cdll.LoadLibrary(dll_file)

class Geometry(object):
    def __init__(self, wkb=None, handle=None):
        self.__handle = handle
        if handle is None:
            if not isinstance(wkb, bytes):
                if hasattr(wkb, 'wkb'):
                    wkb = wkb.wkb
                else:
                    raise RuntimeError("cannot create geometry from "+type(wkb))
            self.__handle = _fourmy.fourmy_from_wkb(wkb)

    def __del__(self):
        self.__handle is not None and _fourmy.fourmy_delete(self.__handle)

    @property
    def handle(self):
        return self.__handle

    @property
    def type(self):
        return {
            0: "Empty",
            1: "Point",
            2: "LineString",
            3: "Polygon",
            4: "MultiPoint",
            5: "MultiLineString",
            6: "MultiPolygon",
            }[_fourmy.fourmy_type(self.__handle)]

    
def tessellate(geom):
    if "Polygon" != geom.type:
        raise RuntimeError("cannot tesselate anything but polygons")
    return Geometry(handle=_fourmy.fourmy_tesselate(geom.handle))



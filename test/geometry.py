from fourmy import tessellate
from shapely.geometry import Polygon, Point, LineString, MultiLineString

t = tessellate(Polygon([(0,0), (1,0), (1,1), (0,1)]))
print(t.wkt)

t = tessellate(Polygon([(0,0), (1,0), (1,1), (0,1)], [[(.2,.2),(.2,.8),(.8,.8),(.8,.2)]]))
print(t.wkt)

t = tessellate(Polygon([(0,0), (1,0), (1,1), (0,1)]), [LineString([(.2, .6), (.8,.6)]), LineString([(.2, .4), (.8,.4)])])
print(t.wkt)

t = tessellate(Polygon([(0,0), (1,0), (1,1), (0,1)]), MultiLineString([LineString([(.2, .6), (.8,.6)]), LineString([(.2, .4), (.8,.4)])]), [Point(.9, .9)])
print(t.wkt)

t = tessellate(Polygon([(0,0), (1,0), (1,1), (0,1)]), lines=None, points=[Point(.9, .9)])
print(t.wkt)

t = tessellate(Polygon(((-4.165589, -29.100525), (8.623957000000001, -28.461553), (21.413503, -27.822581), (10.706928, -13.90117), (0.000353, 0.020242), (-2.082618, -14.540141), (-4.165589, -29.100525))))
print(t.wkt)
assert(len(t) == 4)
print(len(t))
for e in t:
    print(e.area, e.wkt)

t = tessellate(Polygon([(0,0), (1,0), (1,1), (0,1)], [[(.2,.2), (.2,.8), (.8,.8), (.8, .2)]]), lines=[LineString([(.1, .1), (.9,.1)]), LineString([(.9, .1), (.9,.9)]), LineString([(.9, .9), (.1,.9)]), LineString([(.1, .9), (.1,.1)])])
print(len(t))
print(t.wkt)

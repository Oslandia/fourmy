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

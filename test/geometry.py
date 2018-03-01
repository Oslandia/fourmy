from fourmy import tessellate
from shapely.geometry import Polygon, Point

t = tessellate(Polygon([(0,0), (1,0), (1,1), (0,1)]))
print(t.wkt)

t = tessellate(Polygon([(0,0), (1,0), (1,1), (0,1)], [[(.2,.2),(.2,.8),(.8,.8),(.8,.2)]]))
print(t.wkt)



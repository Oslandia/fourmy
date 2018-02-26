from fourmy import Geometry, tessellate
from shapely.geometry import Polygon, Point

print(Geometry(Polygon([(0,0), (1,0), (1,1), (0,1)])).type)
t = tessellate(Geometry(Polygon([(0,0), (1,0), (1,1), (0,1)])))
print(t.type)

Geometry(Point(1,2,3))


## from fourmy import LineString as Ls, loads, tessellate
## from shapely.geometry import Point, LineString, Polygon, MultiPolygon
## #from fourmy import Geometry as Gm, Point as Pt, LineString as Ls
## 
## l = Ls()
## print(len(l))
## 
## print(loads(Point((1,2))))
## #print(loads(Point((1,2,3))))
## #print(Pt(Point((1,2,3)).wkb).coords[0])
## ##assert(Pt(Point((1,2,3)).wkb).z == 3)
## #assert(not Pt(Point((1,2,3)).wkb).has_m)
## #l = Ls(LineString([(1,2,3),(4,5,6)]))
## #c = Ls(LineString([(1,2,3),(4,5,6)])).coords
## #print(c[0])
## #
## #print(l.coords[0][2])
## #print(list(l.coords))
## #Ls(None)
## 
## t = tessellate(Polygon([(0,0), (1,0), (1,1), (0,1)]))
## print(t)
## print(len(t))
## print(t[-1])
## 
## print(Point(t[0][0][0]).wkt)
## 
## print(list(t[0][0]))
## LineString(t[0][0])
## #print(list(MultiPolygon(t)))

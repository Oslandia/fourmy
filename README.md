# fourmy

Fourmy is a minimalistic library to access some CGAL functions from python.

The aim of the library is to be compatible with shapely and to offer functions that are not there.

## usage

```python
from shapely.geometry import Polygon, Point
from fourmy import tessellate

t = tessellate(Polygon([(0,0), (1,0), (1,1), (0,1)]))
print(t.wkt)
```

## install

Fourmy depends on the c++ library CGAL and the python module shapely which sould be installed on your system before running setup script.

```bash
python setup.py build
sudo python setup.py install
```

## history

The necessity arose for contrained triangulation of polygons for wich several libs exists in python, but none was found robust enough for our use case.



from numpy import *

from surf import Surf

class Sphere(Surf):
    def border_curve(self, t):
        t = 2 * pi * t
        return column_stack([cos(t), sin(t), 0.0*t])

    def border_midpoint(self, s1, s2):
        # assume wrapping happens only when one parameter is 0.0
        wrap = logical_or(
                                logical_and(s1 < 1e-10, s2>=0.5),
                                logical_and(s2 < 1e-10, s1>=0.5))
        mid = where(wrap,
            0.5*(s1+s2+1.0),  # unwrapping
            0.5*(s1+s2))
        return mid

    def __init__(self, r=1.0):
        self.r = r
        params = arange(0.0, 1.0, 1.0 / 3)
        self.vertices = self.border_curve(params)
        self.vertices_params = params
        self.edges = array([
            (0, 1), # 0
            (1, 2), # 1
            (2, 0), ]) # 2
        self.is_boundary = full(len(self.edges), True, dtype=bool)
        self.triangles = array([ (0, 1, 2,  0, 0, 0) ])

        self.sanity_check()

        for i in range(5):
            self.triangulate()
            x = self.vertices[:, 0]
            y = self.vertices[:, 1]
            self.vertices = column_stack([x, y, sqrt(maximum(0.0, self.r**2 - x**2 - y**2))])

import numpy as np
#from numba import jit
from math import *

def cross_product(v, w):
    assert len(v) == len(w)
    assert v.shape[1:] == (3, )
    assert w.shape[1:] == (3, )
    r = np.empty((len(v), 3), dtype=v.dtype)
    r[:, 0] = v[:, 1] * w[:, 2] - v[:, 2] * w[:, 1]
    r[:, 1] = v[:, 2] * w[:, 0] - v[:, 0] * w[:, 2]
    r[:, 2] = v[:, 0] * w[:, 1] - v[:, 1] * w[:, 0]
    return r

def dot_product(v, w):
    return v[:,0] * w[:,0] + v[:,1] * w[:,1] + v[:,2] * w[:,2]

def product(v, w):
    return v[:, np.newaxis] * w

def sqr(v):
    return dot_product(v, v)

def abs(v):
    return np.sqrt(sqr(v))

class Surf(object):
    def border_curve(self, t):
        """
        Return an array of shape (len(t), 3) with coordinates of points
        in the boundary with given parameters
        @t: 1d array of parameters
        """
        raise LogicError("{}.border_curve must be implemented".format(type(self)))

    def border_midpoint(self, s1, s2):
        """
        given two 1d-arrays of parameters with same length, compute
        a 1d-arrays of midpoint parameters.
        """
        raise LogicError("{}.border_midpoint must be implemente".format(type(self)))
        # assume wrapping happens only when one parameter is 0.0

    def __init__(self):
        # define empty surface

        # list of vertices:
        self.vertices = np.full((0, 3), 0.0)
        # parameter for boundary vertices or nan:
        self.vertices_params = np.full(len(self.vertices), np.nan)
        # edges (2 indices in vertices):
        self.edges = np.full((0, 2), 0, dtype=int)
        # boundary edges flag:
        self.is_boundary = np.full(len(self.edges), False, dtype=bool)
        # triangles (3 indices in edges + 3 orientation flag [0: positive, 1: negative])
        self.triangles = np.full((0, 6), 0, dtype=int)

    def sanity_check(self):
        # print "sanity_check", len(self.vertices)
        assert self.vertices.shape[1:] == (3, )
        assert self.vertices_params.shape == (len(self.vertices), )
        assert self.edges.shape[1:] == (2, )
        assert np.all(self.edges < len(self.vertices))
        assert self.is_boundary.shape == (len(self.edges), )
        assert self.triangles.shape[1:] == (6, )
        assert np.all(self.triangles[:, :3] < len(self.edges))
        assert np.all(np.logical_or(
            self.triangles[:, 3:] == 0,
            self.triangles[:, 3:] == 1))
        for i in range(3):
            j = (i+1) % 3
            assert np.all(
                self.edges[self.triangles[:,i],1-self.triangles[:,3+i]]
                == self.edges[self.triangles[:,j],self.triangles[:,3+j]])

    def triangle_ratio(self):
        """
        ratio between min edge length and max edge length
        """
        a = self.vertices[self.edges[:,0]]
        b = self.vertices[self.edges[:,1]]
        distance = ((b-a)**2).sum(axis=1) ** 0.5
        max_distance = distance.max()
        min_distance = distance.min()
        return min_distance / max_distance

    def triangle_vertices(self):
        """
        array of shape(len(self.triangles), 3) containing the indices
        of the three vertices of each triangle
        """
        return self.edges[self.triangles[:, :3], self.triangles[:, 3:]]

    def vertex_triangles(self):
        """
        array of shape(len(self.vertices), K) with K = maximum number
        of triangles per vertex, containing the list of triangle indices
        for each vertex multiplied by three and summed to the number of the vertex
        in such triangle. The list is filled with -1.
        """
        N = len(self.triangles)
        a = np.zeros((N*3, 3), dtype=self.triangles.dtype)
        v = self.triangle_vertices()
        a[:N, 0] = v[:, 0]
        a[N:2*N, 0] = v[:, 1]
        a[2*N:, 0] = v[:, 2]
        a[:N, 1] = np.arange(N)*3 + 0
        a[N:2*N, 1] = np.arange(N)*3 + 1
        a[2*N:, 1] = np.arange(N)*3 + 2
        a = a[a[:,0].argsort()]
        a[:,2] = 1
        idx = np.flatnonzero(a[1:, 0] != a[:-1, 0])+1
        a[idx[1:], 2] = idx[:-1] - idx[1:] + 1
        a[idx[0], 2] = -idx[0] + 1
        a[0] = 0
        a[:, 2] = np.cumsum(a[:, 2])
        K = 1 + max(a[:, 2])
        r = np.full((len(self.vertices), K), -1, dtype=int)
        for i in range(K):
            mask = (a[:, 2] == i)
            r[a[mask, 0], i] = a[mask, 1]
        return r

    def triangle_area(self):
        """
        area of each triangle
        """
        tv = self.triangle_vertices()
        v = self.vertices[tv[:,1]] - self.vertices[tv[:,0]]
        w = self.vertices[tv[:,2]] - self.vertices[tv[:,0]]
        n = cross_product(v, w)
        return 0.5 * abs(n)

    def area(self):
        """
        area of surface
        """
        return self.triangle_area().sum()

    def vertex_curvature(self):
        """
        curvature vector of each vertex
        """
        r = np.zeros(self.vertices.shape)
        tv = self.triangle_vertices()
        a = self.triangle_area()
        x = np.zeros((len(self.triangles), 9), dtype=self.vertices.dtype)
        for seq in [[0, 1, 2], [1, 2, 0], [2, 0, 1]]:
            p = self.vertices[tv[:, seq[0]]]
            v = self.vertices[tv[:, seq[1]]] - p
            w = self.vertices[tv[:, seq[2]]] - p
            y = product(dot_product(v, w), (v+w)) - product(sqr(v), w) - product(sqr(w), v)
            y = product(0.5 / a, y)
            assert len(tv) == len(y)
            for i in range(len(tv)):
                r[tv[i, seq[0]], :] += y[i, :]
        return r

    def evolve(self, T=10.0, dt=0.1):
        print "evolve"
        for t in np.arange(0, T, dt):
            k = self.vertex_curvature()
            self.vertices -= dt * k
            print "area", self.area(), "t", t

    def triangulate(self, ratio=0.5):
        """
        @ratio: split all edges which are longer than ratio times the maximum edge length
        """
        # keep old values
        edges = self.edges
        vertices = self.vertices
        triangles = self.triangles

        # compute edge lengths
        a = vertices[edges[:,0]]
        b = vertices[edges[:,1]]
        distance = ((b-a)**2).sum(axis=1) ** 0.5
        max_distance = distance.max()
        del a, b

        # identify long edges to be splitted
        is_long = (distance > ratio * max_distance)
        del distance
        is_long_on_boundary = np.logical_and(is_long, self.is_boundary)
        is_long_internal = np.logical_and(is_long, np.logical_not(self.is_boundary))

        # create new vertices for each long edge
        # use parametric curve for boundary edges!
        midpoints = np.full((len(edges), 3), np.nan)
        midpoints[is_long_internal] = 0.5 * (vertices[edges[is_long_internal][:, 0]]
            + vertices[edges[is_long_internal][:, 1]])
        midpoint_params = self.border_midpoint(
            self.vertices_params[edges[is_long_on_boundary][:, 0]],
            self.vertices_params[edges[is_long_on_boundary][:, 1]])
        midpoints[is_long_on_boundary] = self.border_curve(midpoint_params)

        # map edges to vertex index of midpoint
        midpoint_index = np.where(is_long, is_long.cumsum() - 1 + len(vertices), -1)

        # add midpoints to vertex list
        self.vertices = np.concatenate((
            vertices,
            midpoints[is_long]
            ))
        del vertices
        # store midpoint parameters
        N = is_long.sum()
        m = np.full(len(edges), np.nan, dtype=self.vertices_params.dtype)
        m[is_long_on_boundary] = midpoint_params
        self.vertices_params = np.concatenate((
            self.vertices_params,
            m[is_long]))
        del m

        # create new edges
        # create map to find new half edges from old indices

        # create storage for new edges with a copy of old ones
        self.edges = np.concatenate((edges, edges[is_long]))
        self.is_boundary = np.concatenate((
            self.is_boundary,
            is_long_on_boundary[is_long]))

        assert len(self.edges) == len(self.is_boundary)

        # map edge index to splitted_edge index
        # first column: first half-edge, second column: second half-edge
        splitted_edge_index = np.full((len(edges), 2), -1, dtype=edges.dtype)
        splitted_edge_index[is_long,0] = np.arange(len(edges))[is_long]
        splitted_edge_index[is_long,1] = len(edges) + np.arange(is_long.sum())

        # replace long edges with first half: replace second vertex with midpoint
        self.edges[np.arange(len(edges))[is_long], 1] = midpoint_index[is_long]
        self.edges[len(edges):, 0] = midpoint_index[is_long]

        split_mask = is_long[triangles[:,:3]]
        split_count = split_mask.sum(axis=1)

        # reconstruct self.triangles.
        # First we insert old triangles which have not splitting
        self.triangles = triangles[split_count == 0][:]

        # split triangles with one edge splitted
        split_triangles = triangles[split_count == 1]
        if len(split_triangles):
            # permute triangles edges so that first edge is always the split one
            t = np.full(split_triangles.shape, -1, dtype=split_triangles.dtype)
            mask = split_mask[split_count == 1]
            # "if"s are required to prevent broadcasting error
            if np.any(mask[:,0]):
                t[mask[:,0],:] = split_triangles[mask[:,0],:]
            if np.any(mask[:,1]):
                t[mask[:,1],:] = split_triangles[mask[:,1]][:,[1,2,0,4,5,3]]
            if np.any(mask[:,2]):
                t[mask[:,2],:] = split_triangles[mask[:,2]][:,[2,0,1,5,3,4]]
            split_triangles = t
            del t
            del mask
            N = len(split_triangles)
            # create internal edge from vertex to midpoint
            new_edges = np.full((N, 2), -1, dtype=edges.dtype)
            new_edges[:, 0] = edges[split_triangles[:, 2], split_triangles[:, 5]] # first vertex of third edge
            new_edges[:, 1] = midpoint_index[split_triangles[:, 0]]  # midpoint of first edge
            # create new triangles
            new_triangles = np.full((N*2, 6), -1, dtype=triangles.dtype)
            # first triangle
            new_triangles[:N, 0] = splitted_edge_index[
                split_triangles[:, 0],
                1 - split_triangles[:, 3]]
            new_triangles[:N, 3] = split_triangles[:, 3]
            new_triangles[:N, [1, 4]] = split_triangles[:, [1, 4]]
            new_triangles[:N, 2] = len(self.edges) + np.arange(N)
            new_triangles[:N, 5] = 0
            # second triangle
            new_triangles[N:, 0] = splitted_edge_index[
                split_triangles[:, 0],
                split_triangles[:, 3]]
            new_triangles[N:, 3] = split_triangles[:, 3]
            new_triangles[N:, 1] = len(self.edges) + np.arange(N)
            new_triangles[N:, 4] = 1
            new_triangles[N:, [2, 5]] = split_triangles[:, [2, 5]]

            # update new_edges
            self.edges = np.concatenate((self.edges, new_edges))
            del new_edges
            # extend is_boundary for new internal edges with False values
            x = np.full(len(self.edges), False, dtype=self.is_boundary.dtype)
            x[:len(self.is_boundary)] = self.is_boundary
            self.is_boundary = x
            del x
            # update new triangles
            self.triangles = np.concatenate((
                self.triangles,
                new_triangles))
            del new_triangles

        # TODO: split triangles with two edges splitted
        split_triangles = triangles[split_count == 2]
        if len(split_triangles):
            # permute edges so that first edge is not split
            # permute triangles edges so that first edge is always the split one
            t = np.full(split_triangles.shape, -1, dtype=split_triangles.dtype)
            mask = np.logical_not(split_mask[split_count == 2])
            # "if"s are required to prevent broadcasting error
            if np.any(mask[:,0]):
                t[mask[:,0],:] = split_triangles[mask[:,0],:]
            if np.any(mask[:,1]):
                t[mask[:,1],:] = split_triangles[mask[:,1]][:,[1,2,0,4,5,3]]
            if np.any(mask[:,2]):
                t[mask[:,2],:] = split_triangles[mask[:,2]][:,[2,0,1,5,3,4]]
            split_triangles = t
            del t
            del mask

            # create new edges
            # one edge from midpoints of edges 1 and 2
            # second edge from first point of edge 0 to midpoints of edge 1
            N = len(split_triangles)
            new_edges = np.full((2*N, 2), -1, dtype=edges.dtype)
            new_edges[:N, 0] = midpoint_index[split_triangles[:, 1]]
            new_edges[:N, 1] = midpoint_index[split_triangles[:, 2]]
            new_edges[N:, 0] = edges[split_triangles[:, 0],split_triangles[:, 3]]
            new_edges[N:, 1] = midpoint_index[split_triangles[:, 1]]

            # create new triangles
            # first triangle adjacent to edge 0
            # second one with second half of edge 2
            # third one with vertex opposite to edge 0
            new_triangles = np.full((N*3, 6), -1, dtype=triangles.dtype)
            # first triangle
            new_triangles[:N, [0, 3]] = split_triangles[:, [0, 3]]
            new_triangles[:N, 1] = splitted_edge_index[
                split_triangles[:, 1],
                split_triangles[:, 4]]
            new_triangles[:N, 4] = split_triangles[:, 4]
            new_triangles[:N, 2] = len(self.edges) + N + np.arange(N)
            new_triangles[:N, 5] = 1 # inverted
            # second triangle
            new_triangles[N:2*N, 0] = len(self.edges) + N + np.arange(N)
            new_triangles[N:2*N, 3] = 0
            new_triangles[N:2*N, 1] = len(self.edges) + np.arange(N)
            new_triangles[N:2*N, 4] = 0
            new_triangles[N:2*N, 2] = splitted_edge_index[
                split_triangles[:, 2],
                1-split_triangles[:, 5]]
            new_triangles[N:2*N, 5] = split_triangles[:, 5]
            # third triangle
            new_triangles[2*N:, 0] = splitted_edge_index[
                split_triangles[:, 1],
                1-split_triangles[:, 4]]
            new_triangles[2*N:, 3] = split_triangles[:, 4]
            new_triangles[2*N:, 1] = splitted_edge_index[
                split_triangles[:, 2],
                split_triangles[:, 5]]
            new_triangles[2*N:, 4] = split_triangles[:, 5]
            new_triangles[2*N:, 2] = len(self.edges) + np.arange(N)
            new_triangles[2*N:, 5] = 1 # inverted

            # update new_edges
            self.edges = np.concatenate((self.edges, new_edges))
            del new_edges
            # extend is_boundary for new internal edges with False values
            x = np.full(len(self.edges), False, dtype=self.is_boundary.dtype)
            x[:len(self.is_boundary)] = self.is_boundary
            self.is_boundary = x
            del x
            # update new triangles
            self.triangles = np.concatenate((
                self.triangles,
                new_triangles))
            del new_triangles

        # split triangles with three edges splitted
        split_triangles = triangles[split_count == 3]
        if len(split_triangles):
            N = len(split_triangles)
            # create internal edges. Orient them so that new
            # internal triangle is counterclockwise
            new_edges = np.full((N*3, 2), -1, dtype=edges.dtype)
            new_edges[::3] = midpoint_index[split_triangles[:,[1, 2]]]
            new_edges[1::3] = midpoint_index[split_triangles[:, [2, 0]]]
            new_edges[2::3] = midpoint_index[split_triangles[:, [0, 1]]]
            # every triangle split3 triangle is divided into 4 small triangles
            new_triangles = np.full((N*4, 6), -1, dtype=triangles.dtype)

            # first we consider the small triangle opposite to edge i
            for i in range(3):
                new_triangles[i*N:(i+1)*N, 0] = splitted_edge_index[
                    split_triangles[:, (i+1)%3],  # edge
                    1 - split_triangles[:, 3+(i+1)%3]]  # second half if positive orientation
                new_triangles[i*N:(i+1)*N, 3] = split_triangles[:, 3+(i+1)%3] # same orientation
                new_triangles[i*N:(i+1)*N, 1] = splitted_edge_index[
                    split_triangles[:, (i+2)%3], # edge
                    split_triangles[:, 3+(i+2)%3]] # first half if positive orientation
                new_triangles[i*N:(i+1)*N, 4] = split_triangles[:, 3+(i+2)%3] # same orientation
                new_triangles[i*N:(i+1)*N, 2] = len(self.edges) + np.arange(i,3*N,3) # new edge
                new_triangles[i*N:(i+1)*N, 5] = 1 # centrale edge always inverted
            # then the internal triangle
            new_triangles[3*N: 4*N, 0] = len(self.edges) + np.arange(0,3*N,3) # first edge
            new_triangles[3*N: 4*N, 1] = len(self.edges) + np.arange(1,3*N,3)
            new_triangles[3*N: 4*N, 2] = len(self.edges) + np.arange(2,3*N,3)
            new_triangles[3*N: 4*N, 3:6] = 0  # new edges are oriented properly

            # update new_edges
            self.edges = np.concatenate((self.edges, new_edges))
            del new_edges
            # extend is_boundary for new internal edges with False values
            x = np.full(len(self.edges), False, dtype=self.is_boundary.dtype)
            x[:len(self.is_boundary)] = self.is_boundary
            self.is_boundary = x
            del x
            # update new triangles
            self.triangles = np.concatenate((
                self.triangles,
                new_triangles))
            del new_triangles

        self.sanity_check()

    def obj_lines(self):
        yield "# surf object"
        yield "g Object001"
        yield ""
        for v in self.vertices:
            yield "v {} {} {}".format(v[0], v[1], v[2])
        yield ""
        for t in self.triangles:
            yield "f {}".format(' '.join(map(str, [1+self.edges[i][j] for i, j in zip(t[:3], t[3:])])))

    def write_obj(self, filename):
        with open(filename, "w") as out:
            for line in self.obj_lines():
                out.write(line)
                out.write('\n')

if __name__ == '__main__':
    from test_surf import test, TestSurf
    test()
    from sphere import Sphere
    surf = Sphere()
    k = surf.vertex_curvature()
    print dot_product(k, surf.vertices)
    print abs(k)
    surf.write_obj("surf.obj")

import numpy as np
from math import *

class Surf(object):
    def border_curve(self, t):
        t = 2 * pi * t
        return np.column_stack([np.cos(t), np.sin(t), 0.1*np.cos(2*t)])

    def border_midpoint(self, s1, s2):
        # assume wrapping happens only when one parameter is 0.0
        return np.where(np.logical_or(
                                np.logical_and(s1==0.0, s2>=0.75),
                                np.logical_and(s2==0.0, s1>=0.75)),
            0.5*(s1+s2+1.0),  # unwrapping
            0.5*(s1+s2))

    def __init__(self):
        params = np.array([
            0.0,  # 0
            0.25, # 1
            0.5,  # 2
            0.75, # 3
            ])
        self.vertices = self.border_curve(params)
        self.vertices_params = np.full(len(self.vertices), np.nan)
        self.vertices_params[np.arange(4)] = params
        self.edges = np.array([
            (0, 1), # 0
            (1, 2), # 1
            (2, 3), # 2
            (3, 0), # 3
            (0, 2), # 4
            ]) # indices in self.vertices
        self.is_boundary = np.full(len(self.edges), False, dtype=bool)
        self.is_boundary[np.arange(4)] = True # indexed as in self.edges
        self.triangles = np.array([
            (0, 1, 4,   0, 0, 1),
            (4, 2, 3,   0, 0, 0),
            ]) # indices in self.edges + orientation 0/1

        self.sanity_check()

    def triangle_ratio(self):
        a = self.vertices[self.edges[:,0]]
        b = self.vertices[self.edges[:,1]]
        distance = ((b-a)**2).sum(axis=1) ** 0.5
        max_distance = distance.max()
        min_distance = distance.min()
        return min_distance / max_distance

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
    from test_surf import test_triangulation
    test_triangulation()
    surf = Surf()
    r = 0.6
    surf.triangulate(ratio=r)
    surf.triangulate(ratio=r)
    surf.triangulate(ratio=r)
    surf.triangulate(ratio=r)
    surf.triangulate(ratio=r)
    surf.write_obj("surf.obj")

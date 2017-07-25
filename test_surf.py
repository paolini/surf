from numpy import nan, array, all, allclose

from surf import Surf

def test_triangulation():
    surf = Surf()
    surf.triangulate()
    assert allclose(surf.vertices, array([
        [  1.00000000e+00,   0.00000000e+00,   1.00000000e-01],
        [  6.12323400e-17,   1.00000000e+00,  -1.00000000e-01],
        [ -1.00000000e+00,   1.22464680e-16,   1.00000000e-01],
        [ -1.83697020e-16,  -1.00000000e+00,  -1.00000000e-01],
        [  7.07106781e-01,   7.07106781e-01,   6.12323400e-18],
        [ -7.07106781e-01,   7.07106781e-01,  -1.83697020e-17],
        [ -7.07106781e-01,  -7.07106781e-01,   3.06161700e-17],
        [  7.07106781e-01,  -7.07106781e-01,  -4.28626380e-17],
        [  0.00000000e+00,   6.12323400e-17,   1.00000000e-01]]))
    assert allclose(surf.vertices_params,
    array([ 0.   ,  0.25 ,  0.5  ,  0.75 ,  0.125,  0.375,  0.625,  0.875,
      nan]), equal_nan=True)
    assert all(surf.edges == array([
        [0, 4],[1, 5],[2, 6],[3, 7],[0, 8],[4, 1],
        [5, 2],[6, 3],[7, 0],[8, 2],[5, 8],[8, 4],
        [4, 5],[6, 7],[7, 8],[8, 6]]))
    assert all(surf.is_boundary == array([
        True,  True,  True,  True, False,  True,  True,  True,  True,
        False, False, False, False, False, False, False]))
    assert all(surf.triangles == array(
        [[ 6,  9, 10,  0,  1,  1],
         [ 7,  3, 13,  0,  0,  1],
         [ 4,  0, 11,  1,  0,  1],
         [ 8,  4, 14,  0,  0,  1],
         [ 5,  1, 12,  0,  0,  1],
         [ 9,  2, 15,  0,  0,  1],
         [10, 11, 12,  0,  0,  0],
         [13, 14, 15,  0,  0,  0]]))

    print "test_triangulation 1 passed"

    surf.triangulate(0.6)

    assert allclose(surf.vertices, array([
           [  1.00000000e+00,   0.00000000e+00,   1.00000000e-01],
           [  6.12323400e-17,   1.00000000e+00,  -1.00000000e-01],
           [ -1.00000000e+00,   1.22464680e-16,   1.00000000e-01],
           [ -1.83697020e-16,  -1.00000000e+00,  -1.00000000e-01],
           [  7.07106781e-01,   7.07106781e-01,   6.12323400e-18],
           [ -7.07106781e-01,   7.07106781e-01,  -1.83697020e-17],
           [ -7.07106781e-01,  -7.07106781e-01,   3.06161700e-17],
           [  7.07106781e-01,  -7.07106781e-01,  -4.28626380e-17],
           [  0.00000000e+00,   6.12323400e-17,   1.00000000e-01],
           [  5.00000000e-01,   3.06161700e-17,   1.00000000e-01],
           [ -5.00000000e-01,   9.18485099e-17,   1.00000000e-01],
           [ -3.53553391e-01,   3.53553391e-01,   5.00000000e-02],
           [  3.53553391e-01,   3.53553391e-01,   5.00000000e-02],
           [  5.55111512e-17,   7.07106781e-01,  -6.12323400e-18],
           [ -1.11022302e-16,  -7.07106781e-01,  -6.12323400e-18],
           [  3.53553391e-01,  -3.53553391e-01,   5.00000000e-02],
           [ -3.53553391e-01,  -3.53553391e-01,   5.00000000e-02]]))

    assert allclose(surf.vertices_params, array([ 0.   ,  0.25 ,  0.5  ,  0.75 ,  0.125,  0.375,  0.625,  0.875,
              nan,    nan,    nan,    nan,    nan,    nan,    nan,    nan,
              nan]), equal_nan=True)

    assert all(surf.edges == array([
           [ 0,  4],
           [ 1,  5],
           [ 2,  6],
           [ 3,  7],
           [ 0,  9],
           [ 4,  1],
           [ 5,  2],
           [ 6,  3],
           [ 7,  0],
           [ 8, 10],
           [ 5, 11],
           [ 8, 12],
           [ 4, 13],
           [ 6, 14],
           [ 7, 15],
           [ 8, 16],
           [ 9,  8],
           [10,  2],
           [11,  8],
           [12,  4],
           [13,  5],
           [14,  7],
           [15,  8],
           [16,  6],
           [ 3, 14],
           [ 1, 13],
           [10, 11],
           [12,  9],
           [ 9, 15],
           [16, 10],
           [ 5, 10],
           [ 0, 12],
           [ 7,  9],
           [ 2, 16],
           [12, 13],
           [13, 11],
           [11, 12],
           [15, 16],
           [16, 14],
           [14, 15]]))

    assert all(surf.is_boundary == array([
           True,  True,  True,  True, False,  True,  True,  True,  True,
           False, False, False, False, False, False, False, False, False,
           False, False, False, False, False, False, False, False, False,
           False, False, False, False, False, False, False, False, False,
           False, False, False, False], dtype=bool))

    assert all(surf.triangles == array([
           [13,  7, 24,  1,  0,  0],
           [12,  5, 25,  1,  0,  0],
           [21, 24,  3,  1,  1,  0],
           [20, 25,  1,  1,  1,  0],
           [ 6, 17, 30,  0,  1,  1],
           [ 0, 19, 31,  0,  1,  1],
           [ 8,  4, 32,  0,  0,  1],
           [ 2, 23, 33,  0,  1,  1],
           [30, 26, 10,  0,  0,  1],
           [31, 27,  4,  0,  0,  1],
           [32, 28, 14,  0,  0,  1],
           [33, 29, 17,  0,  0,  0],
           [ 9, 18, 26,  1,  1,  1],
           [11, 16, 27,  1,  1,  1],
           [16, 22, 28,  0,  1,  1],
           [15,  9, 29,  1,  0,  1],
           [19, 12, 34,  0,  0,  1],
           [22, 15, 37,  0,  0,  1],
           [20, 10, 35,  0,  0,  1],
           [23, 13, 38,  0,  0,  1],
           [18, 11, 36,  0,  0,  1],
           [21, 14, 39,  0,  0,  1],
           [34, 35, 36,  0,  0,  0],
           [37, 38, 39,  0,  0,  0]]))

    print "test_triangulation 2 passed"
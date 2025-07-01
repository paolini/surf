import {Vector, scale_vector, vector_sum, vector_diff, number_product, vector_product, squared_norm, compute_area} from './algebra'
import * as THREE from 'three'

type Index = number

type Volume = {
    surfaces: [Index, number][] // list of surface indices and orientation
    pressure: number // pressure inside the volume
}

export default class Surf {
    name: string
    
    // memory on GPU
    buffer: ArrayBuffer 
    
    // list of vertex coordinates
    // x1 y1 z1 x2 y2 z2 ...
    // n_vertices = vertices.length / 3
    vertices: Float32Array 

    // list of surfaces
    // each surface is a list of vertex indices for triangles
    // a1 b1 c1 a2 b2 c2 ...
    // coordinates of vertex with index k
    // are vertices[3*k], vertices[3*k+1], vertices[3*k+2]
    // n_triangles = indices.length / 3
    surfaces: Index[][]

    // for each surface 
    // list of pressure on triangles (given by volumes)
    surfacesPressures: Float32Array[] | null

    // for each surface
    // list of coordinates of area x normal for each triangle
    surfacesNormalAreaVectors: Float32Array[] | null

    // list of areas of fans around each vertex
    vertexAreas: Float32Array | null

    // list of fixed borders
    // each border is a list of vertex indices and parameter values
    // f(t) is used for smooth interpolation
    borders: ({
        f: (t:number)=>Vector,
        indices: [Index,number][]
        period: number,
        closed: boolean
    })[]

    volumes: Volume[]

    constructor(name) {
        this.name = name
        this.buffer = new ArrayBuffer(0)
        this.vertices = new Float32Array()
        this.surfaces = []
        this.borders = []
        this.volumes = []
        this.surfacesPressures = null
        this.surfacesNormalAreaVectors = null
        this.vertexAreas = null
        
        this.newSurface()
    }

    newSurface() {
        const n = this.surfaces.length
        this.surfaces.push([])
        return n
    }

    /**
     * Create a new border with given parameterization
     * @param f parameterization function
     * @param period period of parameter if curve is closed, 0 if it is open
     * @returns the index of the newly created border
     */
    addBorder(f:((number)=>Vector), period: number = 1.0, closed: boolean=true): Index {
        const borderIndex = this.borders.length
        const indices = []
        this.borders.push({f, indices, period, closed})
        return borderIndex
    }

    resizeVertices(n: Index) {
        const bufferSize = this.buffer.byteLength
        const requiredSize = 4*3*n
        if (bufferSize < requiredSize) {     
            const newBufferSize = Math.max(2*bufferSize,requiredSize)
            console.log("reallocating", newBufferSize)
            this.buffer = new ArrayBuffer(newBufferSize)
            const newVertices = new Float32Array(this.buffer, 0, 3*n)
            newVertices.set(this.vertices)
            this.vertices = newVertices
        } else {
            this.vertices = new Float32Array(this.buffer, 0, 3*n)
        }
    }

    addVertex(x:number, y:number, z:number) {
        const n = this.vertices.length / 3
        this.resizeVertices(n+1)
        this.vertices[3*n] = x
        this.vertices[3*n + 1] = y
        this.vertices[3*n + 2] = z
        return n
    }

    getVertex(i: Index): Vector {
        return [this.vertices[3*i], this.vertices[3*i+1], this.vertices[3*i+2]]
    }

    /*** 
     * add a vertex to a boundary
     * borderCount: is the boundary number
     * t: is the parameter along the border curve
     * vertexIndex: if provided add an already existing vertex to the boundary
     */
    addBorderVertex(t: number, borderCount: Index = 0, vertexIndex: Index = -1): Index {
        const border = this.borders[borderCount]
        const p = border.f(t)
        if (vertexIndex<0) vertexIndex = this.addVertex(...p)
        const d = squared_norm(vector_diff(p,this.getVertex(vertexIndex)))
        if (d>0.001) throw Error("mismatching vertices in addBorderVertex")
        border.indices.push([vertexIndex,t])
        return vertexIndex
    }

    addTriangle(surface_index:Index, a:Index, b:Index, c:Index) {
        this.surfaces[surface_index].push(a, b, c)
    }

    addQuad(surface_index:Index,a:Index,b:Index,c:Index,d:Index) {
        this.surfaces[surface_index].push(a,b,c)
        this.surfaces[surface_index].push(a,c,d)
    }

    distanceBetweenVerticesSquared(a:Index, b:Index):number {
        const x1 = this.vertices[3*a]
        const y1 = this.vertices[3*a + 1]
        const z1 = this.vertices[3*a + 2]
        const x2 = this.vertices[3*b]
        const y2 = this.vertices[3*b + 1]
        const z2 = this.vertices[3*b + 2]
        const d = (x2-x1)**2 + (y2-y1)**2 + (z2-z1)**2
        return d
    }

    computeMaxSquaredEdgeLength(): number {
        var maxEdgeLength = 0
        const self=this
        this.surfaces.forEach(function(indices:Index[]) {
            const n = indices.length / 3
            for (var t=0; t<n ; t++) {
                const a = indices[3*t]
                const b = indices[3*t + 1]
                const c = indices[3*t + 2]
                maxEdgeLength = Math.max(maxEdgeLength, 
                    self.distanceBetweenVerticesSquared(a,b),
                    self.distanceBetweenVerticesSquared(b,c),
                    self.distanceBetweenVerticesSquared(c,a))
            }
        })
        return maxEdgeLength
    }

    triangulateToR(r: number=0) {
        if (r===0) {
            this.triangulateOnceToR()
            return
        }
        
        while(!this.triangulateOnceToR(r)) {}
    }

    triangulateOnceToR(r:number = 0): boolean {
        this.surfacesPressures = null

        const maxSquaredEdgeLength = this.computeMaxSquaredEdgeLength()
        const targetSquaredLength = r>0 ? Math.max(r*r,0.5*maxSquaredEdgeLength) : 0.5*maxSquaredEdgeLength
        
        // console.log({maxEdgeLengthSquared,"sqrt":Math.sqrt(maxEdgeLengthSquared),targetLengthSquared,r,rr:r*r})
        
        if (r>0 && (maxSquaredEdgeLength < r*r)) return true // below threshold
        
        
        let lastIndex = this.vertices.length / 3
        const newVertices: {[key:string]:Vector} = {} 
        
        const computeKey = (a,b) => {
            if (a>b) return `${b}:${a}`
            else return `${a}:${b}`
        }
        
        const split = (a, b) => {
            const dd = this.distanceBetweenVerticesSquared(a,b)
            if (dd < targetSquaredLength) return
            const key = computeKey(a,b)
            if (key in newVertices) return
            newVertices[key] = [a,b,lastIndex++]
        }

        this.surfaces = this.surfaces.map(function(indices) {
            const newIndices: number[] = []

            for (let t=0 ; t<indices.length ; t+=3) {
                split(indices[t], indices[t+1])
                split(indices[t+1], indices[t+2])
                split(indices[t+2], indices[t])
            }

            for (let t=0 ; t<indices.length ; t+=3) {
                const [a,b,c] = [indices[t], indices[t+1], indices[t+2]]
                const mid_ab = newVertices[computeKey(a,b)]
                const mid_bc = newVertices[computeKey(b,c)]
                const mid_ca = newVertices[computeKey(c,a)]
                if (mid_ab) {
                    const ab = mid_ab[2]
                    if (mid_bc) {
                        const bc = mid_bc[2]
                        if (mid_ca) {
                            const ca = mid_ca[2]
                            newIndices.push(ab, bc, ca)
                            newIndices.push(a, ab, ca)
                            newIndices.push(ab, b, bc)
                            newIndices.push(ca, bc, c)
                        } else {
                            newIndices.push(a, ab, c)
                            newIndices.push(ab, bc, c)
                            newIndices.push(ab, b, bc)
                        }
                    } else {
                        if (mid_ca) {
                            const ca = mid_ca[2]
                            newIndices.push(c, ca, b)
                            newIndices.push(ca, ab, b)
                            newIndices.push(ca, a, ab)
                        } else {
                            newIndices.push(a, ab, c)
                            newIndices.push(ab, b, c)
                        }
                    }
                } else {
                    if (mid_bc) {
                        const bc = mid_bc[2]
                        if (mid_ca) {
                            const ca = mid_ca[2]
                            newIndices.push(b, bc, a)
                            newIndices.push(bc, ca, a)
                            newIndices.push(bc, c, ca)
                        } else {
                            newIndices.push(a, b, bc)
                            newIndices.push(a, bc, c)
                        }
                    } else {
                        if (mid_ca) {
                            const ca = mid_ca[2]
                            newIndices.push(a, b, ca)
                            newIndices.push(b, c, ca)
                        } else {
                            newIndices.push(a, b, c)
                        }
                    }
                }
            }
            // console.log(`newIndices: ${newIndices}`)
            return newIndices        
        })

        // console.log(`newVertices: ${JSON.stringify(newVertices)}`)
        const values: Vector[] = Object.values(newVertices)
        this.resizeVertices(this.vertices.length/3 + values.length)
        const vertices = this.vertices
        const offset = this.vertices.length 
        for(let i=0; i<values.length; ++i) {
            let [a,b,c] = values[i]
            for (let j=0 ; j<3; j++) {
                vertices[c*3 + j] = 0.5*(vertices[3*a+j]+vertices[3*b+j])
            }
        }

        this.borders.forEach(border => {
            const indices = border.indices
            const newIndices: [number,number][] = []
            for (let i=0; i<indices.length; ++i) {
                newIndices.push(indices[i])
                const ii = i<indices.length-1 ? i+1 : 0
                const key = computeKey(indices[i][0], indices[ii][0])
                const v = newVertices[key]
                if (!v) continue
                const [a,b,c] = v
                const t = indices[i][1]
                const tt = indices[ii][1]
                let ttt = 0.5*(t+tt)
                if (border.closed && Math.abs(tt-t) > 0.5*border.period) {
                    ttt -= 0.5*border.period
                    if (ttt<0) ttt += border.period
                }
                newIndices.push([c, ttt])
                const xyz = border.f(ttt)
                for (let j=0; j<3; j++) {
                    vertices[c*3 + j] = xyz[j]
                }
            }
            border.indices = newIndices
        })
        
        return false
    }

    equalizeTriangles() {
        const vertices = this.vertices

        console.log(`equalizeTriangles`)
        console.log(`n_vertices: ${this.vertices.length/3}`)

        function distance_squared(v1,v2) {
            const dx = vertices[3*v2+0] - vertices[3*v1+0]
            const dy = vertices[3*v2+1] - vertices[3*v1+1]
            const dz = vertices[3*v2+2] - vertices[3*v1+2]
            return dx*dx + dy*dy + dz*dz 
        }

        this.surfaces = this.surfaces.map(indices => {
            const n_triangles = indices.length / 3
            console.log(`n_triangles: ${n_triangles}`)
            function hash(index1, index2) {
                return index1 <= index2
                    ? `${index1}:${index2}`
                    : `${index2}:${index1}`
            }

            type EdgeData = {
                v1: number, v2: number, v3: number, v4: number,
                t1: number, t2: number,
                // v4<0, t2<0: single triangle found so far
                // v3<0: don't flip...
            }
            const edge_data: EdgeData[] = []

            /* store information of the triangle relative 
             * to the edge v1,v2
             */

            function store_triangle(t,v1,v2,v3) {
                //if (v1>v2) [v1, v2] = [v2, v1]
                const h = hash(v1,v2)
                const data = edge_data[h]
                if (data === undefined) {
                    edge_data[h] = {
                        v1, v2, v3,
                        v4: -1,
                        t1: t,
                        t2: -1
                    }
                } else if (data.v4 < 0) {
                    data.t2 = t
                    data.v4 = v3
                    // second triangle
                    const d = distance_squared(v1,v2)
                    const dd = distance_squared(data.v3, v3)
                    if (d <= dd) data.v3 = -1 // don't flip
                } else {
                    data.v3 = -1 // don't flip: too many triangles on this edge
                }
            }

            for (let t=0; t<indices.length; t+=3) {
                store_triangle(t, indices[t], indices[t+1], indices[t+2])
                store_triangle(t, indices[t+1], indices[t+2], indices[t])
                store_triangle(t, indices[t+2], indices[t], indices[t+1])                
            }

            // operate
            const new_indices = [...indices]
            let modified = false
            Object.values(edge_data).forEach(data => {
                if (data.v3 >= 0 && data.v4 >= 0) {
                    // require flip

                    // invalidate adjacent edges
                    function invalidate(v,ww) {
                        const h = hash(v,ww)
                        const data = edge_data[h]
                        if (data) data.v3 = -1 // keep
                    }                    
                    invalidate(data.v1,data.v3)
                    invalidate(data.v1,data.v4)
                    invalidate(data.v2,data.v3)
                    invalidate(data.v2,data.v4)
                    
                    new_indices[data.t1] = data.v1
                    new_indices[data.t1+1] = data.v4
                    new_indices[data.t1+2] = data.v3
                    new_indices[data.t2] = data.v2
                    new_indices[data.t2+1] = data.v3
                    new_indices[data.t2+2] = data.v4 
                    modified = true
                }
            })

            return modified ? new_indices : indices
        })
    }

    printVertices() {
        const vertices = this.vertices
        for (let i=0; 3*i<vertices.length; i++) {
            console.log(`${i}: (${vertices[3*i]},${vertices[3*i+1]},${vertices[3*i+2]})`)
        }
    }

    computeArea(surface_n:Index): number {
        let area = 0.0
        const vertices = this.vertices
        const indices = this.surfaces[surface_n]

        for (let t=0; t<indices.length; t+=3) {
            const [a,b,c] = [indices[t], indices[t+1], indices[t+2]]
            const [xa,ya,za] = [vertices[3*a], vertices[3*a+1], vertices[3*a+2]]
            const [xb,yb,zb] = [vertices[3*b], vertices[3*b+1], vertices[3*b+2]]
            const [xc,yc,zc] = [vertices[3*c], vertices[3*c+1], vertices[3*c+2]]
            const [xab,yab,zab] = [xb-xa, yb-ya, zb-za]
            const [xac,yac,zac] = [xc-xa, yc-ya, zc-za]
            const [xabxac,yabyac,zabzac] = [yab*zac - zab*yac, zab*xac - xab*zac, xab*yac - yab*xac]
            area += 0.5*Math.sqrt(xabxac*xabxac + yabyac*yabyac + zabzac*zabzac)
        }
        return area
    }

    computeMeanCurvatureVector(): Float32Array {
        // and also computes this.surfacesNormalAreaVectors and this.vertexAreas

        this.surfacesNormalAreaVectors = this.surfaces.map((indices) => new Float32Array(indices.length))
        const surfacesNormalAreaVectors = this.surfacesNormalAreaVectors

        const vertices = this.vertices
        const n_vertices = vertices.length / 3
        this.vertexAreas = new Float32Array(n_vertices)
        const vertexAreas = this.vertexAreas

        const meanCurvatureVector = new Float32Array(n_vertices*3)

        for (let s=0; s<this.surfaces.length;s++) {
            const indices = this.surfaces[s]
            const normals = surfacesNormalAreaVectors[s]
            if (normals.length !== indices.length) throw Error("normals length mismatch")

            for (let t=0; t<indices.length; t+=3) {
                const [a,b,c] = [indices[t], indices[t+1], indices[t+2]]
                const [xa,ya,za] = [vertices[3*a], vertices[3*a+1], vertices[3*a+2]]
                const [xb,yb,zb] = [vertices[3*b], vertices[3*b+1], vertices[3*b+2]]
                const [xc,yc,zc] = [vertices[3*c], vertices[3*c+1], vertices[3*c+2]]
                const [xab,yab,zab] = [xb-xa, yb-ya, zb-za]
                const [xac,yac,zac] = [xc-xa, yc-ya, zc-za]
                const [xbc,ybc,zbc] = [xc-xb, yc-yb, zc-zb]
                const [vx,vy,vz] = [yab*zac - zab*yac, zab*xac - xab*zac, xab*yac - yab*xac]

                // store normals in buffer for later use
                normals[t  ] = vx
                normals[t+1] = vy
                normals[t+2] = vz

                const area2 = Math.sqrt(vx*vx + vy*vy + vz*vz)

                vertexAreas[a] += area2
                vertexAreas[b] += area2
                vertexAreas[c] += area2

                const acbc = (xac*xbc + yac*ybc + zac*zbc)
                const abbc = (xab*xbc + yab*ybc + zab*zbc)
                const abac = (xab*xac + yab*yac + zab*zac)

                meanCurvatureVector[3*a]   += (-acbc*xab + abbc*xac)
                meanCurvatureVector[3*a+1] += (-acbc*yab + abbc*yac)
                meanCurvatureVector[3*a+2] += (-acbc*zab + abbc*zac)
                meanCurvatureVector[3*b]   += (-abac*xbc + acbc*xab)
                meanCurvatureVector[3*b+1] += (-abac*ybc + acbc*yab)
                meanCurvatureVector[3*b+2] += (-abac*zbc + acbc*zab)
                meanCurvatureVector[3*c]   += (-abbc*xac + abac*xbc)
                meanCurvatureVector[3*c+1] += (-abbc*yac + abac*ybc)
                meanCurvatureVector[3*c+2] += (-abbc*zac + abac*zbc)
            }
        }

        for (let i=0; i<n_vertices; i++) {
            const j = 3*i;
            const c = 8/vertexAreas[i]
            meanCurvatureVector[j]   *= c
            meanCurvatureVector[j+1] *= c
            meanCurvatureVector[j+2] *= c
        }

        for (let i=0; i<this.borders.length; ++i) {
            for (let j=0; j<this.borders[i].indices.length; ++j) {
                const n = this.borders[i].indices[j][0]
                meanCurvatureVector[3*n] = 0.0
                meanCurvatureVector[3*n+1] = 0.0
                meanCurvatureVector[3*n+2] = 0.0
            }
        }
        return meanCurvatureVector
    }

    /*
    computeNormalVector(): Float32Array {
        const vertices = this.vertices
        const n = vertices.length / 3
        const normalVector = new Float32Array(n*3)

        if (this.surfacesNormalAreaVectors === null) throw Error("surfacesNormalAreaVectors not computed")
        
        for (let s=0;s<this.surfaces.length;s++) {
            const indices = this.surfaces[s]
            const areaNormals = this.surfacesNormalAreaVectors[s]

            for (let t=0; t<indices.length; t+=3) {
                const [a,b,c] = [indices[t], indices[t+1], indices[t+2]]
                const [vx,vy,vz] = [areaNormals[t], areaNormals[t+1], areaNormals[t+2]]
                const area2 = Math.sqrt(vx*vx + vy*vy + vz*vz)

                normalVector[3*a]   += vx / area2
                normalVector[3*a+1] += vy / area2
                normalVector[3*a+2] += vz / area2
                normalVector[3*b]   += vx / area2
                normalVector[3*b+1] += vy / area2
                normalVector[3*b+2] += vz / area2
                normalVector[3*c]   += vx / area2
                normalVector[3*c+1] += vy / area2
                normalVector[3*c+2] += vz / area2
            }
        }

        // normalize normal vectors
        for (let i=0; i<n; ++i) {
            const d = Math.sqrt(
                normalVector[3*i]*normalVector[3*i] 
                + normalVector[3*i+1]*normalVector[3*i+1] 
                + normalVector[3*i+2]*normalVector[3*i+2])
            normalVector[3*i] /= d
            normalVector[3*i+1] /= d
            normalVector[3*i+2] /= d
        }

        return normalVector
    }*/

    computeSurfacePressures() {
        const surfacesPressures = this.surfaces.map((indices) => new Float32Array(indices.length/3))

        this.volumes.forEach(volume => {
            volume.surfaces.forEach(([surface, orientation]) => {
                const pressures = surfacesPressures[surface]
                for (let i=0; i<pressures.length; i++) {
                    pressures[i] += orientation*volume.pressure
                }
            })
        })

        this.surfacesPressures = surfacesPressures
    }

    computePressureForces(): Float32Array {
        const vertices = this.vertices
        const n_vertices = vertices.length / 3
        const pressureForces = new Float32Array(n_vertices*3)
        const surfacesPressures = this.surfacesPressures
        const surfacesNormalAreaVectors = this.surfacesNormalAreaVectors
        const vertexAreas = this.vertexAreas

        if (surfacesPressures === null) throw Error("surfacesPressures not computed")
        if (surfacesNormalAreaVectors === null) throw Error("surfacesNormalAreaVectors not computed")
        if (vertexAreas === null) throw Error("vertexAreas not computed")

        this.surfaces.forEach((indices, surface_n) => {
            // indices is a flat list of vertex indices triples
            const pressures = surfacesPressures[surface_n]
            const normals = surfacesNormalAreaVectors[surface_n]
            const n_triangles = indices.length / 3
            if (pressures.length !== n_triangles) throw Error("pressures length mismatch")
            if (normals.length !== 3*n_triangles) throw Error("normals length mismatch")

            for(let t=0;t<n_triangles;t++) {
                const [a,b,c] = [indices[3*t], indices[3*t+1], indices[3*t+2]]
                const pressure = pressures[t]
                const [nx,ny,nz] = [normals[3*t], normals[3*t+1], normals[3*t+2]]

                pressureForces[3*a  ] += pressure*nx
                pressureForces[3*a+1] += pressure*ny
                pressureForces[3*a+2] += pressure*nz
                pressureForces[3*b  ] += pressure*nx
                pressureForces[3*b+1] += pressure*ny
                pressureForces[3*b+2] += pressure*nz
                pressureForces[3*c  ] += pressure*nx
                pressureForces[3*c+1] += pressure*ny
                pressureForces[3*c+2] += pressure*nz

            }
        })

        if (vertexAreas.length !== n_vertices) throw Error("vertexAreas length mismatch")

        for (let i=0; i<n_vertices; i++) {
            const c = 1/vertexAreas[i]
            pressureForces[3*i  ] *= c
            pressureForces[3*i+1] *= c
            pressureForces[3*i+2] *= c
        }

        return pressureForces
    }

    evolveVector(dt: number, vector: Float32Array) {
        const vertices = this.vertices
        for (let i=0; i<vertices.length; i+=3) {
            vertices[i] -= dt*vector[i]
            vertices[i + 1] -= dt*vector[i + 1]
            vertices[i + 2] -= dt*vector[i + 2]
        }
    }

    evolve(dt: number, count: number = 1) {
        if (this.volumes && this.volumes.length > 0 && this.surfacesPressures === null) this.computeSurfacePressures()

        while(count>0) {
            const vector = this.computeMeanCurvatureVector()
            if (this.volumes.length>0) {
                const pressureForces = this.computePressureForces()
                if (pressureForces.length !== vector.length) throw Error("pressureForces length mismatch")
                for (let i=0; i<vector.length; i++) {
                    vector[i] -= pressureForces[i] // dovrebbe esserci il segno opposto ?!?
                }
            }
            this.evolveVector(dt, vector)
            count--
        }
    }

    run() {
        this.triangulateToR(1)
        this.evolve(0.05,20)
        this.triangulateToR(0.25)
        this.evolve(0.05,20)
    }

    /**
     * Export the surface to STL format
     * @returns STL content as a string
     */
    exportToSTL(): string {
        let stl = 'solid surf\n';
        const vertices = this.vertices;

        for (let s=0;s<this.surfaces.length;++s) {
            const indices = this.surfaces[s];

            for (let i = 0; i < indices.length; i += 3) {
                const [a, b, c] = [indices[i], indices[i + 1], indices[i + 2]];
                const va: Vector = [vertices[3 * a], vertices[3 * a + 1], vertices[3 * a + 2]];
                const vb: Vector = [vertices[3 * b], vertices[3 * b + 1], vertices[3 * b + 2]];
                const vc: Vector = [vertices[3 * c], vertices[3 * c + 1], vertices[3 * c + 2]];

                const normal = computeNormal(va, vb, vc);

                stl += `facet normal ${normal[0]} ${normal[1]} ${normal[2]}\n`;
                stl += `  outer loop\n`;
                stl += `    vertex ${va[0]} ${va[1]} ${va[2]}\n`;
                stl += `    vertex ${vb[0]} ${vb[1]} ${vb[2]}\n`;
                stl += `    vertex ${vc[0]} ${vc[1]} ${vc[2]}\n`;
                stl += `  endloop\n`;
                stl += `endfacet\n`;
            }
        }

        stl += 'endsolid surf\n';
        return stl;
    }

    exportToPovRay(camera: THREE.camera): string {
        const vertices = this.vertices
        const normalVector = this.computeMeanCurvatureVector() // to compute normals!

        let out:string = ''

        function out_vertex(a: number) {
            out += `<${vertices[3*a]},${vertices[3*a+1]},${vertices[3*a+2]}> `
        }

        function out_normal(a: number) {
            out += `<${normalVector[3*a]},${normalVector[3*a+1]},${normalVector[3*a+2]}> `
        }

        function out_triangle(a: number, b: number, c: number) {        
            // smooth_triangle
            // {
            //     <Corner_1>, <Normal_1>, <Corner_2>,
            //     <Normal_2>, <Corner_3>, <Normal_3>
            //     [OBJECT_MODIFIER...]
            // } 
            out += "smooth_triangle {\n"
            out_vertex(a)
            out_normal(a)
            out_vertex(b)
            out_normal(b)
            out_vertex(c)
            out_normal(c)
            out += "}\n"
        }
              
        const r_scale: number=0.001
        function out_line(a: number,b: number, r:number) {
            out += "cylinder { "
            out_vertex(a)
            out += ", "
            out_vertex(b)
            out += ", "
            out += r*r_scale
            out += " open texture {Wire}}\n"
            out += "sphere { "
            out_vertex(a)
            out += ", "
            out += r*r_scale
            out += " texture {Wire}}\n"
        }
      
        function out_vector(v: Vector) {
            out += `<${v[0]},${v[1]},${v[2]}> `
        }

        function out_color(x: Vector) {
            out += "color rgb "
            out += `<${x[0]},${x[1]},${x[2]}> `
        }
      
        out += `// POVRAY file generated by surf manu-fatto\n`
      
        out += "camera{location "
        out_vector(camera.position.toArray())
        out += "\n"
        out += "sky "
        out_vector(camera.up)
        out += "\n"
        out += "look_at "
        out_vector(camera.lookAt)
        out += "}\n\n"
      
        out += "light_source{ "
        out_vector([0,0,10])
        out_color([1,1,1])
        out += "}\n"
      
        // out += "#include \"metals.inc\"\n"
        out += "#declare P_Chrome2 = color rgb <0.39, 0.41, 0.43>;\n\n"        
        out += `
            #ifdef THIS_IS_OLD
                "#declare Color = texture {\n"
                "pigment{color rgbf <0.8,0.8,1,0.3>}\n"
                "finish{irid{0.25 thickness 0.7 turbulence 0.3}\n"
                "  ambient .4 phong 0.75}\n"
                "}\n\n"
                "#declare Wire = texture {\n"
                "pigment{P_Chrome2}\n"
                "finish{ambient .4 phong 0.75}\n}\n\n"
            #else
                "#declare Color = texture {\n"
                "pigment{color rgbf <0.75,0.8,1.0,0.3>}\n"
                "finish{irid{0.2 thickness 0.7 turbulence 0.6}\n"
                "  ambient .6 phong 0.75}\n"
                "}\n\n"
                "#declare Wire = texture {\n"
                "pigment{P_Chrome2}\n"
                "finish{ambient 0 phong 0.75}\n}\n\n"
            #endif
        `
        out += "union{\n"
        out += "// plane { <0,0,1>, -5 pigment {color rgb <1,1,1>}\n"
        out += "// finish {ambient 0.5}}\n"
    
        this.surfaces.forEach(function(triangles) {
            for (let t=0; t<triangles.length; t+=3) {
                out_triangle(triangles[t], triangles[t+1], triangles[t+2])
            }
        })

        out += "texture {Color}\n"
        out += "}\n\n//END\n"

        return out
      }
}

/**
 * Compute the normal vector for a triangle
 * @param a vertex A
 * @param b vertex B
 * @param c vertex C
 * @returns Normal vector as [nx, ny, nz]
 */
function computeNormal(a: Vector, b: Vector, c: Vector): Vector {
    const ab = vector_diff(b, a)
    const ac = vector_diff(c, a)
    const n = vector_product(ab, ac)

    const length = Math.sqrt(squared_norm(n))
    return scale_vector(1 / length, n)
}


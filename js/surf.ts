import {Vector, scale_vector, vector_sum, vector_diff, number_product, squared_norm, compute_area} from './algebra'

type Index = number

export default class Surf {
    name: string
    buffer: ArrayBuffer
    vertices: Float32Array
    indices: Index[]
    borders: ({
        f: (t:number)=>Vector,
        indices: [Index,number][]
        period: number,
        closed: boolean
    })[]

    constructor(name) {
        this.name = name
        this.buffer = new ArrayBuffer(0)
        this.vertices = new Float32Array()
        this.indices = []
        this.borders = []
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

    addTriangle(a:Index, b:Index, c:Index) {
        this.indices.push(a, b, c)
    }

    addQuad(a:Index,b:Index,c:Index,d:Index) {
        this.indices.push(a,b,c)
        this.indices.push(a,c,d)
    }

    squaredDistanceBetweenVertices(a:Index, b:Index):number {
        const x1 = this.vertices[3*a]
        const y1 = this.vertices[3*a + 1]
        const z1 = this.vertices[3*a + 2]
        const x2 = this.vertices[3*b]
        const y2 = this.vertices[3*b + 1]
        const z2 = this.vertices[3*b + 2]
        const d = Math.sqrt( (x2-x1)**2 + (y2-y1)**2 + (z2-z1)**2 )
        return d
    }

    computeMaxEdgeLengthSquared(): number {
        var maxEdgeLength = 0
        const n = this.indices.length / 3
        for (var t=0; t<n ; t++) {
            const a = this.indices[3*t]
            const b = this.indices[3*t + 1]
            const c = this.indices[3*t + 2]
            maxEdgeLength = Math.max(maxEdgeLength, 
                this.squaredDistanceBetweenVertices(a,b),
                this.squaredDistanceBetweenVertices(b,c),
                this.squaredDistanceBetweenVertices(c,a))
        }
        return maxEdgeLength
    }

    triangulate(r: number=0) {
        if (r===0) {
            this.triangulateOnce()
            return
        }
        
        while(!this.triangulateOnce(r)) {}
    }

    triangulateOnce(r:number = 0): boolean {
        const indices = this.indices
        const maxEdgeLengthSquared = this.computeMaxEdgeLengthSquared()
        const targetLengthSquared = r>0 ? Math.max(r*r,0.5*maxEdgeLengthSquared) : 0.5*maxEdgeLengthSquared

        // console.log({maxEdgeLengthSquared,"sqrt":Math.sqrt(maxEdgeLengthSquared),targetLengthSquared,r,rr:r*r})

        if (r>0 && (maxEdgeLengthSquared < r*r)) return true // below threshold


        let lastIndex = this.vertices.length / 3
        const newVertices: {[key:string]:Vector} = {} 
        const newIndices: number[] = []
         
        const computeKey = (a,b) => {
            if (a>b) return `${b}:${a}`
            else return `${a}:${b}`
        }

        const split = (a, b) => {
            const d = this.squaredDistanceBetweenVertices(a,b)
            if (d < targetLengthSquared) return
            const key = computeKey(a,b)
            if (key in newVertices) return
            newVertices[key] = [a,b,lastIndex++]
        }

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
        // console.log(`newVertices: ${JSON.stringify(newVertices)}`)
        // console.log(`newIndices: ${newIndices}`)
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
        
        this.indices=newIndices
        return false
    }

    printVertices() {
        const vertices = this.vertices
        for (let i=0; 3*i<vertices.length; i++) {
            console.log(`${i}: (${vertices[3*i]},${vertices[3*i+1]},${vertices[3*i+2]})`)
        }
    }

    computeArea(): number {
        let area = 0.0
        const vertices = this.vertices
        const indices = this.indices

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
        const vertices = this.vertices
        const indices = this.indices
        const n = vertices.length / 3
        const meanCurvatureVector = new Float32Array(n*3)

        for (let t=0; t<indices.length; t+=3) {
            const [a,b,c] = [indices[t], indices[t+1], indices[t+2]]
            const [xa,ya,za] = [vertices[3*a], vertices[3*a+1], vertices[3*a+2]]
            const [xb,yb,zb] = [vertices[3*b], vertices[3*b+1], vertices[3*b+2]]
            const [xc,yc,zc] = [vertices[3*c], vertices[3*c+1], vertices[3*c+2]]
            const [xab,yab,zab] = [xb-xa, yb-ya, zb-za]
            const [xac,yac,zac] = [xc-xa, yc-ya, zc-za]
            const [xbc,ybc,zbc] = [xc-xb, yc-yb, zc-zb]
            const [xabxac,yabyac,zabzac] = [yab*zac - zab*yac, zab*xac - xab*zac, xab*yac - yab*xac]
            const area2 = Math.sqrt(xabxac*xabxac + yabyac*yabyac + zabzac*zabzac)

            const acbc = xac*xbc + yac*ybc + zac*zbc
            const abbc = xab*xbc + yab*ybc + zab*zbc
            const abac = xab*xac + yab*yac + zab*zac

            meanCurvatureVector[3*a]   += (-acbc*xab + abbc*xac) / area2
            meanCurvatureVector[3*a+1] += (-acbc*yab + abbc*yac) / area2
            meanCurvatureVector[3*a+2] += (-acbc*zab + abbc*zac) / area2
            meanCurvatureVector[3*b]   += (-abac*xbc + acbc*xab) / area2
            meanCurvatureVector[3*b+1] += (-abac*ybc + acbc*yab) / area2
            meanCurvatureVector[3*b+2] += (-abac*zbc + acbc*zab) / area2
            meanCurvatureVector[3*c]   += (-abbc*xac + abac*xbc) / area2
            meanCurvatureVector[3*c+1] += (-abbc*yac + abac*ybc) / area2
            meanCurvatureVector[3*c+2] += (-abbc*zac + abac*zbc) / area2

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

    evolveVector(dt: number, meanCurvatureVector: Float32Array) {
        const vertices = this.vertices
        for (let i=0; i<vertices.length; i+=3) {
            vertices[i] -= dt*meanCurvatureVector[i]
            vertices[i + 1] -= dt*meanCurvatureVector[i + 1]
            vertices[i + 2] -= dt*meanCurvatureVector[i + 2]
        }
    }

    evolveMeanCurvature(dt: number, count: number = 1) {
        while(count>0) {
            const vector = this.computeMeanCurvatureVector()
            this.evolveVector(dt, vector)
            count--
        }
    }

    run() {
        this.triangulate(1)
        this.evolveMeanCurvature(0.05,20)
        this.triangulate(0.5)
        this.evolveMeanCurvature(0.05,20)
    }

    /**
     * Export the surface to STL format
     * @returns STL content as a string
     */
    exportToSTL(): string {
        let stl = 'solid surf\n';
        const vertices = this.vertices;
        const indices = this.indices;

        for (let i = 0; i < indices.length; i += 3) {
            const [a, b, c] = [indices[i], indices[i + 1], indices[i + 2]];
            const [ax, ay, az] = [vertices[3 * a], vertices[3 * a + 1], vertices[3 * a + 2]];
            const [bx, by, bz] = [vertices[3 * b], vertices[3 * b + 1], vertices[3 * b + 2]];
            const [cx, cy, cz] = [vertices[3 * c], vertices[3 * c + 1], vertices[3 * c + 2]];

            const normal = this.computeNormal(ax, ay, az, bx, by, bz, cx, cy, cz);

            stl += `facet normal ${normal[0]} ${normal[1]} ${normal[2]}\n`;
            stl += `  outer loop\n`;
            stl += `    vertex ${ax} ${ay} ${az}\n`;
            stl += `    vertex ${bx} ${by} ${bz}\n`;
            stl += `    vertex ${cx} ${cy} ${cz}\n`;
            stl += `  endloop\n`;
            stl += `endfacet\n`;
        }

        stl += 'endsolid surf\n';
        return stl;
    }

    /**
     * Compute the normal vector for a triangle
     * @param ax x-coordinate of vertex A
     * @param ay y-coordinate of vertex A
     * @param az z-coordinate of vertex A
     * @param bx x-coordinate of vertex B
     * @param by y-coordinate of vertex B
     * @param bz z-coordinate of vertex B
     * @param cx x-coordinate of vertex C
     * @param cy y-coordinate of vertex C
     * @param cz z-coordinate of vertex C
     * @returns Normal vector as [nx, ny, nz]
     */
    private computeNormal(ax: number, ay: number, az: number, bx: number, by: number, bz: number, cx: number, cy: number, cz: number): [number, number, number] {
        const abx = bx - ax;
        const aby = by - ay;
        const abz = bz - az;
        const acx = cx - ax;
        const acy = cy - ay;
        const acz = cz - az;

        const nx = aby * acz - abz * acy;
        const ny = abz * acx - abx * acz;
        const nz = abx * acy - aby * acx;

        const length = Math.sqrt(nx * nx + ny * ny + nz * nz);
        return [nx / length, ny / length, nz / length];
    }
}

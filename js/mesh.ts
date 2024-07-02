type Vector = [number, number, number]

export default class Mesh {
    buffer: ArrayBuffer
    vertices: Float32Array
    indices: number[]
    borders: ({
        f: (t:number)=>Vector,
        indices: [number,number][]
        period: number,
    })[]

    constructor() {
        this.buffer = new ArrayBuffer(0)
        this.vertices = new Float32Array()
        this.indices = []
        this.borders = []
    }

    addBorder(f:((number)=>Vector), period: number) {
        this.borders.push({
            f: f,
            indices: [],
            period: period,
        })
    }

    resizeVertices(n) {
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

    addVertex(x, y, z) {
        const n = this.vertices.length / 3
        this.resizeVertices(n+1)
        this.vertices[3*n] = x
        this.vertices[3*n + 1] = y
        this.vertices[3*n + 2] = z
        return n
    }

    addBorderVertex(t: number, borderCount: number = 0) {
        const border = this.borders[borderCount]
        const n = this.addVertex(...border.f(t))
        border.indices.push([n,t])
        return n
    }

    addTriangle(a, b, c) {
        this.indices.push(a, b, c)
    }

    squaredDistanceBetweenVertices(a, b) {
        const x1 = this.vertices[3*a]
        const y1 = this.vertices[3*a + 1]
        const z1 = this.vertices[3*a + 2]
        const x2 = this.vertices[3*b]
        const y2 = this.vertices[3*b + 1]
        const z2 = this.vertices[3*b + 2]
        return Math.sqrt( (x2-x1)**2 + (y2-y1)**2 + (z2-z1)**2 )
    }

    computeMaxEdgeLengthSquared() {
        var maxEdgeLength = 0
        const n = this.vertices.length / 3
        for (var t=0; t<n ; t++) {
            const a = this.indices[3*t]
            const b = this.indices[3*t + 1]
            const c = this.indices[3*t + 2]
            maxEdgeLength = Math.max(maxEdgeLength, this.squaredDistanceBetweenVertices(a,b))
            maxEdgeLength = Math.max(maxEdgeLength, this.squaredDistanceBetweenVertices(b,c))
            maxEdgeLength = Math.max(maxEdgeLength, this.squaredDistanceBetweenVertices(c,a))
        }
        return maxEdgeLength
    }

    triangulate() {
        const indices = this.indices
        const nTriangles = indices.length / 3
        const maxEdgeLengthSquared = this.computeMaxEdgeLengthSquared()

        console.log(`old indices: ${indices}`)
        this.printVertices()

        let lastIndex = this.vertices.length / 3
        const newVertices: {[key:string]:Vector} = {} 
        const newIndices: number[] = []
         
        const computeKey = (a,b) => {
            if (a>b) return `${b}:${a}`
            else return `${a}:${b}`
        }

        const split = (a, b) => {
            const d = this.squaredDistanceBetweenVertices(a,b)
            if (d < 0.5*maxEdgeLengthSquared) return
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
        console.log(`newVertices: ${JSON.stringify(newVertices)}`)
        console.log(`newIndices: ${newIndices}`)
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
            for (let i=0; i<indices.length; ++i) {
                const ii = i<indices.length-1 ? i+1 : 0
                const key = computeKey(indices[i][0], indices[ii][0])
                const v = newVertices[key]
                if (!v) continue
                const [a,b,c] = v
                const t = indices[i][1]
                const tt = indices[ii][1]
                let ttt = 0.5*(t+tt)
                if (Math.abs(tt-t) > 0.5*border.period) {
                    ttt -= 0.5*border.period
                    if (ttt<0) ttt += border.period
                }
                const xyz = border.f(ttt)
                for (let j=0; j<3; j++) {
                    vertices[c*3 + j] = xyz[j]
                }
            }
        })
        
        this.indices=newIndices
    }

    evolve() {
        const N_TRIANGLES = this.indices.length / 3
        const N_VERTICES = this.vertices.length / 3

        function vertex(i: number): Vector {
            return [this.vertices[3*i], this.vertices[3*i+1], this.vertices[3*i+2]]
        }

        for (let i=0; i<N_TRIANGLES; i++) {
            const a = this.indices[3*i]
            const b = this.indices[3*i+1]
            const c = this.indices[3*i+2]
            const area = compute_area(vertex(a), vertex(b), vertex(c))

            /* WIP, see vertex.cc vertex::grad */
            const v = vector_diff(vertex(b), vertex(a))
            const w = vector_diff(vertex(c), vertex(a))


            const grad: Vector = scale_vector(1/(2*area), 
                vector_diff(scale_vector(number_product(v,w),vector_sum(v,w)), 
                    vector_sum(
                        scale_vector(squared_norm(v),w),
                        scale_vector(squared_norm(w),v))))

            /* WIP */
        }
    }

    printVertices() {
        const vertices = this.vertices
        for (let i=0; 3*i<vertices.length; i++) {
            console.log(`${i}: (${vertices[3*i]},${vertices[3*i+1]},${vertices[3*i+2]})`)
        }
    }
}

function compute_area(a: Vector, b: Vector, c:Vector) {
    const [vx, vy, vz] = [b[0]-a[0], b[1]-a[1], b[2]-a[2]]
    const [wx, wy, wz] = [c[0]-a[0], c[1]-a[1], c[2]-a[2]]
    return 0.5*Math.sqrt((vy*wz-vz*wy)**2 + (vz*wx-vx*wz)**2 + (vx*wy-vy*wx)**2)
}

function vector_diff(a: Vector, b: Vector): Vector {
    return [a[0]-b[0], a[1]-b[1], a[2]-b[2]]
}

function vector_sum(a: Vector, b: Vector): Vector {
    return [a[0]+b[0], a[1]+b[1], a[2]+b[2]]
}

function scale_vector(b: number, a: Vector): Vector {
    return [a[0]*b, a[1]*b, a[2]*b]
}

function number_product(a: Vector, b: Vector): number {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2]
}

function squared_norm(a: Vector): number {
    return number_product(a,a)
}
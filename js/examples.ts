import Surf from './surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin
const sqrt = Math.sqrt

export class Pringle extends Surf {
    constructor(h:number=0.5,n:number=2) {
        super()

        this.addBorder(t => 
            [  cos(t), 
                sin(t), 
                h*sin(n*t)], 2*PI)

        this.addBorderVertex(0) // v0
        this.addBorderVertex(2*PI/3) // v1
        this.addBorderVertex(4*PI/3) // v2
    
        this.addTriangle(0, 1, 2)
    }

    run() {
        this.triangulate(1)
        this.evolveMeanCurvature(0.05,20)
        this.triangulate(0.5)
        this.evolveMeanCurvature(0.05,20)
    }
}

export class Catenoid extends Surf {
    constructor(R:number=1, h:number=1) {
        super()

        this.addBorder(t => [R*cos(t), R*sin(t),0], 2*PI)
        this.addBorder(t => [R*cos(t), R*sin(t),h], 2*PI)

        this.addBorderVertex(0,0) // 0
        this.addBorderVertex(2*PI/3, 0) // 1
        this.addBorderVertex(4*PI/3, 0) // 2
        this.addBorderVertex(0,1) // 3
        this.addBorderVertex(2*PI/3, 1) // 4
        this.addBorderVertex(4*PI/3, 1) // 5

        this.addQuad(0,1,4,3)
        this.addQuad(1,2,5,4)
        this.addQuad(2,0,3,5)
    }
}

export class Cube extends Surf {
    constructor(simply_connected: boolean = true,sx: number = 1.0,sy: number = 1.0,sz: number = 1.0) {
      super()

      // 8 vertices cube 
      const v000 = this.addVertex(0,0,0)
      const v001 = this.addVertex(0,0,sz)
      const v010 = this.addVertex(0,sy,0)
      const v011 = this.addVertex(0,sy,sz)
      const v100 = this.addVertex(sx,0,0)
      const v101 = this.addVertex(sx,0,sz)
      const v110 = this.addVertex(sx,sy,0)
      const v111 = this.addVertex(sx,sy,sz)

      // 4 vertices square (m=mid)
      const vm00 = this.addVertex(0.5*sx,0.3*sy,0.3*sz)
      const vm01 = this.addVertex(0.5*sx,0.3*sy,0.7*sz)
      const vm10 = this.addVertex(0.5*sx,0.7*sy,0.3*sz)
      const vm11 = this.addVertex(0.5*sx,0.7*sy,0.7*sz)

      // 12 borders
      const bx00 = this.addBorder(t => [t*sx, 0, 0], 1.0, false)
      const bx01 = this.addBorder(t => [t*sx, 0,sz], 1.0, false)
      const bx10 = this.addBorder(t => [t*sx,sy, 0], 1.0, false)
      const bx11 = this.addBorder(t => [t*sx,sy,sz], 1.0, false)
      const by00 = this.addBorder(t => [ 0,t*sy, 0], 1.0, false)
      const by01 = this.addBorder(t => [sx,t*sy, 0], 1.0, false)
      const by10 = this.addBorder(t => [ 0,t*sy,sz], 1.0, false)
      const by11 = this.addBorder(t => [sx,t*sy,sz], 1.0, false)
      const bz00 = this.addBorder(t => [ 0, 0,t*sz], 1.0, false)
      const bz01 = this.addBorder(t => [ 0,sy,t*sz], 1.0, false)
      const bz10 = this.addBorder(t => [sx, 0,t*sz], 1.0, false)
      const bz11 = this.addBorder(t => [sx,sy,t*sz], 1.0, false)
      
      // fix border vertices
      this.addBorderVertex(0,bx00,v000)
      this.addBorderVertex(1,bx00,v100)
      this.addBorderVertex(0,bx01,v001)
      this.addBorderVertex(1,bx01,v101)
      this.addBorderVertex(0,bx10,v010)
      this.addBorderVertex(1,bx10,v110)
      this.addBorderVertex(0,bx11,v011)
      this.addBorderVertex(1,bx11,v111)
      this.addBorderVertex(0,by00,v000)
      this.addBorderVertex(1,by00,v010)
      this.addBorderVertex(0,by01,v100)
      this.addBorderVertex(1,by01,v110)
      this.addBorderVertex(0,by10,v001)
      this.addBorderVertex(1,by10,v011)
      this.addBorderVertex(0,by11,v101)
      this.addBorderVertex(1,by11,v111)
      this.addBorderVertex(0,bz00,v000)
      this.addBorderVertex(1,bz00,v001)
      this.addBorderVertex(0,bz01,v010)
      this.addBorderVertex(1,bz01,v011)
      this.addBorderVertex(0,bz10,v100)
      this.addBorderVertex(1,bz10,v101)
      this.addBorderVertex(0,bz11,v110)
      this.addBorderVertex(1,bz11,v111)
      
      // facets 4+4+1 quads, 4 triangles
      this.addQuad(v000,vm00,vm10,v010)
      this.addQuad(v010,vm10,vm11,v011)
      this.addQuad(v011,vm11,vm01,v001)
      this.addQuad(v001,vm01,vm00,v000)
      this.addQuad(vm00,v100,v110,vm10)
      this.addQuad(vm10,v110,v111,vm11)
      this.addQuad(vm11,v111,v101,vm01)
      this.addQuad(vm01,v101,v100,vm00)
      this.addTriangle(v000,v100,vm00)
      this.addTriangle(v001,v101,vm01)
      this.addTriangle(v010,v110,vm10)
      this.addTriangle(v011,v111,vm11)
      if (simply_connected)
        this.addQuad(vm00,vm10,vm11,vm01)
    }
  }
    
export class Helicoid extends Surf {
  constructor(R:number=1.0,r:number=0, h:number=1.0, N:number=2) {
    super()
  
    const PERIOD = 4.0
    this.addBorder(t => {
      if (t<2.0) {
        if (t<1.0) { // t in [0,1]
	        return [r+t*(R-r),0,-h]
        } else { // t in [1,2]
	        const a=N*2.0*PI*(t-1.0)
	        return [R*cos(a),R*sin(a),(t-1.5)*2.0*h]
        }
      } else {
        if (t<3.0) { // t in [2,3]
	        const a=N*2.0*PI
	        const s=(3.0-t)*(R-r)
	        return [(r+s)*cos(a),(r+s)*sin(a),h]
        } else { // t in [3,4]
	        const a=N*2.0*PI*(4.0-t)
	        return [r*cos(a),r*sin(a),(3.5-t)*2.0*h]
        }
      }
    }, PERIOD)

    this.addBorderVertex(0)
    this.addBorderVertex(PERIOD/3)
    this.addBorderVertex(2*PERIOD/3)
    this.addTriangle(0,1,2)
  }

  run() {
    for (let i=0; i<10; i++) {
      this.triangulate()
      this.evolveMeanCurvature(0.05,20)
    }
  }
}
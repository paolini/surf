import { Vector } from './algebra'
import Surf from './surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin
const sqrt = Math.sqrt

export class Pringle extends Surf {
    constructor(h:number=0.5,n:number=2) {
        super(`pringle  h:${h} n:${n}`)

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
        super(`catenoid R:${R} h:${h}`)

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
      super(`cube sx:${sx} sy:${sy} sz:${sz} simply_connected:${simply_connected}`)

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
    super(`helicoid R:${R} r:${r} h:${h} N:${N}`)
  
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

export class Manu extends Surf {
  constructor(depth:number=0.5, R:number=1.0, r:number=0.4, h:number=0.1, radius:number=0.7) { 
    super(`manu depth:${depth} R:${R} r:${r} h:${h} radius:${radius}`)
    let p:[number,number][] = []
    const self = this

    function double_vertex(x,y) {
      p.push([
        self.addVertex(x,y,-depth),
        self.addVertex(x,y,depth)])
    }
  
    function side(a, b) {
      self.addTriangle(p[a][0],p[b][0],p[b][1])
      self.addTriangle(p[a][0],p[b][1],p[a][1])
    }
    
    function triangle(a, b, c) {
      self.addTriangle(p[c][0],p[b][0],p[a][0])
      self.addTriangle(p[a][1],p[b][1],p[c][1]);
    }
  
    function quad(a, b, c, d) {
      triangle(a,b,c)
      triangle(a,c,d)
    }
    
    function perimeter() {
      const nv = p.length
      for(let i=0;i<nv;i++) {
        side(i,(i+1)%nv)
      }
    }
  
    function build(s)	{
      let x=0
      for(let i=0;i<s.length;i++,x+=6) {
        console.log(`Writing: ${s[i]}`)
        p = []
        switch(s[i]) {
          case 'E':
            double_vertex(x+0,0)
            double_vertex(x+5,0)
            double_vertex(x+5,1)
            double_vertex(x+1,1)
            double_vertex(x+1,2)
            double_vertex(x+3,2)
            double_vertex(x+3,3)
            double_vertex(x+1,3)
            double_vertex(x+1,4)
            double_vertex(x+5,4)
            double_vertex(x+5,5)
            double_vertex(x+0,5)
            perimeter()
            quad(0,1,2,3)
            quad(4,5,6,7)
            quad(0,3,8,11)
            quad(8,9,10,11)
            break
          case 'M':
            double_vertex(x+0,0)
            double_vertex(x+1,0)
            double_vertex(x+1,3)
            double_vertex(x+2.5,2.5)
            double_vertex(x+4,3)
            double_vertex(x+4,0)
            double_vertex(x+5,0)
            double_vertex(x+5,5)
            double_vertex(x+2.5,4)
            double_vertex(x+0,5)
            perimeter()
            quad(0,1,2,9)
            quad(2,3,8,9)
            quad(3,4,7,8)
            quad(4,5,6,7)
            break
          case 'A':
            double_vertex(x+0,0)
            double_vertex(x+1,0)
            double_vertex(x+1.4,1)
            double_vertex(x+3.6,1)
            double_vertex(x+4,0)
            double_vertex(x+5,0)
            double_vertex(x+3,5)
            double_vertex(x+2,5)
            perimeter()
            double_vertex(x+1.8,2)
            double_vertex(x+3.2,2)
            double_vertex(x+2.5,5.0-1.25)
            side(8,9)
            side(9,10)
            side(10,8)
            quad(0,1,10,7)
            quad(2,3,9,8)
            quad(4,5,6,10)
            triangle(10,6,7)
            break
          case 'N':
            double_vertex(x+0,0)
            double_vertex(x+1,0)
            double_vertex(x+1,3.5)
            double_vertex(x+4,0)
            double_vertex(x+5,0)
            double_vertex(x+5,5)
            double_vertex(x+4,5)
            double_vertex(x+4,1.5)
            double_vertex(x+1,5)
            double_vertex(x+0,5)
            perimeter()
            quad(0,1,8,9)
            quad(2,3,7,8)
            quad(3,4,5,6)
            break
          case 'U':
            double_vertex(x+1,2.5)
            double_vertex(x+1,5)
            double_vertex(x+0,5)
            double_vertex(x+0,2.5)
            double_vertex(x+4,2.5)
            double_vertex(x+4,5)
            double_vertex(x+5,5)
            double_vertex(x+5,2.5)
            quad(0,1,2,3)
            quad(7,6,5,4)
            side(0,1)
            side(1,2)
            side(2,3)
            side(7,6)
            side(6,5)
            side(5,4)
            const n = p.length // n=8
            const N = 20
            let a = 0
            let b = 3
            for (let j = 1; j<N; j++) {
              const theta = PI + j*PI/N
              console.log(`theta=${theta} j=${j} cos=${cos(theta)} sin=${sin(theta)}`)
              double_vertex(x+2.5+2.5*cos(theta),2.5+2.5*sin(theta))
              double_vertex(x+2.5+1.5*cos(theta),2.5+1.5*sin(theta))
              let c = p.length-1
              let d = p.length-2
              side(c,a)
              side(b,d)
              quad(c,a,b,d)
              a=c
              b=d
              }
            side(4,a)
            side(b,7)
            quad(4,a,b,7)
            break
          case 'L':
            double_vertex(x+0,0)
            double_vertex(x+5,0)
            double_vertex(x+5,1)
            double_vertex(x+1,1)
            double_vertex(x+1,5)
            double_vertex(x+0,5)
            perimeter()
            quad(0,1,2,3)
            quad(3,4,5,0)
            break
          }
        }
      return x;
    }

    const l = build("NUELEMA")
  
    function transform(v:Vector):Vector {
      const [x,y,z] = v
      const vx = x*2.0*PI/l
      const vy = (y-2.5)/5.0
      const a=r*vy*cos(vx/2.0)+h*z*sin(vx/2.0)
      const b=h*z*cos(vx/2.0)-r*vy*sin(vx/2.0)
      return [(R+a)*cos(vx),(R+a)*sin(vx),b]
      }
    
    this.triangulate(radius)

    for(let i=0;i<this.vertices.length;i+=3) {
      [ this.vertices[i],
        this.vertices[i+1],
        this.vertices[i+2]
      ] = transform([
        this.vertices[i],
        this.vertices[i+1],
        this.vertices[i+2]])
    }
  }

  run(){}
}

export class Marco extends Surf {
  constructor(r:number=1.0, R:number=1.5, h:number=0.7, osc:number=0.1) {
    super(`marco r:${r} R:${R} h:${h} osc:${osc}`)

    this.addBorder(t=>[r*cos(t),r*sin(t),h-osc*cos(2.0*t)], 2*PI)
    this.addBorder(t=>[R*cos(t),R*sin(t),osc*cos(2.0*t)], 2*PI)
    this.addBorder(t=>[r*cos(t),r*sin(t),-h-osc*cos(2.0*t)], 2*PI)

    const [t0,t1,t2,t3] = [0,PI/2,PI,3*PI/2]
    const p00 = this.addBorderVertex(t0,0)
    const p01 = this.addBorderVertex(t1,0)
    const p02 = this.addBorderVertex(t2,0)
    const p03 = this.addBorderVertex(t3,0)
    const p10 = this.addBorderVertex(t0,1)
    const p11 = this.addBorderVertex(t1,1)
    const p12 = this.addBorderVertex(t2,1)
    const p13 = this.addBorderVertex(t3,1)
    const p20 = this.addBorderVertex(t0,2)
    const p21 = this.addBorderVertex(t1,2)
    const p22 = this.addBorderVertex(t2,2)
    const p23 = this.addBorderVertex(t3,2)
 
    const q = this.addVertex(0,0,0)
    this.addQuad(p01,p02,p12,p11)
    this.addQuad(p03,p00,p10,p13)
    this.addQuad(p20,p21,p11,p10)
    this.addQuad(p22,p23,p13,p12)

    this.addTriangle(q,p10,p00)
    this.addTriangle(q,p20,p10)
    this.addTriangle(q,p00,p01)
    this.addTriangle(q,p01,p11)
    this.addTriangle(q,p11,p21)
    this.addTriangle(q,p21,p22)
    this.addTriangle(q,p12,p02)
    this.addTriangle(q,p22,p12)
    this.addTriangle(q,p02,p03)
    this.addTriangle(q,p03,p13)
    this.addTriangle(q,p13,p23)
    this.addTriangle(q,p23,p20)
 }
}

class MoebiusBase extends Surf {
  constructor(R:number=1.0, r:number=0.4) {
    super(`moebius R:${R} r:${r}`)
    /* t va da 0 a 4\pi */
    this.addBorder(
      t => [(R-r*cos(t/2.0))*cos(t),(R-r*cos(t/2.0))*sin(t),r*(sin(t/2.0))],
      4*PI)
  }

  computeP() {
    return [
      this.addBorderVertex(2*PI/6),
      this.addBorderVertex(6*PI/6),
      this.addBorderVertex(10*PI/6),
      this.addBorderVertex(14*PI/6),
      this.addBorderVertex(18*PI/6),
      this.addBorderVertex(22*PI/6)]
  }
}

export class Moebius extends MoebiusBase {
  constructor(R:number=1.0, r:number=0.4) {
    super(R,r)
    const p = this.computeP()
    this.addQuad(p[0],p[1],p[4],p[3])
    this.addQuad(p[1],p[2],p[5],p[4])
    this.addQuad(p[2],p[3],p[0],p[5])
  }
}

export class MoebiusOriented extends MoebiusBase {
  constructor(R:number=1.0, r:number=0.4) {
    super(R,r)
    this.name += " oriented"
    const p = this.computeP()
    this.addTriangle(p[4],p[3],p[5])
    this.addQuad(p[0],p[2],p[3],p[5])
    this.addTriangle(p[0],p[1],p[2])
  }
}  

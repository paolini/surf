import { Vector } from '../algebra'
import Surf from '../surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin

export class Manu extends Surf {
  constructor(depth:number=0.5, R:number=1.0, r:number=0.4, h:number=0.1, radius:number=0.5) { 
    super(`manu depth:${depth} R:${R} r:${r} h:${h} radius:${radius}`)
    let p:[number,number][] = []
    const self = this

    function double_vertex(x,y) {
      p.push([
        self.addVertex(x,y,-depth),
        self.addVertex(x,y,depth)])
    }
  
    function side(a, b) {
      self.addTriangle(0, p[a][0],p[b][0],p[b][1])
      self.addTriangle(0, p[a][0],p[b][1],p[a][1])
    }
    
    function triangle(a, b, c) {
      self.addTriangle(0, p[c][0],p[b][0],p[a][0])
      self.addTriangle(0, p[a][1],p[b][1],p[c][1]);
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
    this.triangulateToR(radius)

    if (true) { // build connecting moebius strip
      const dx = 0.2
      const y = 1.5
      const yy = 2.5
      const z = 0.2

      function rectangle(x) {
        return [
          self.addVertex(x,yy-y,-z),
          self.addVertex(x,yy+y,-z),
          self.addVertex(x,yy+y,z),
          self.addVertex(x,yy-y,z)
        ]
      }

      const first_q = rectangle(0)
      let last_q = first_q
      for (let x=dx;x<l;x+=dx) {
        const q = rectangle(x)
        this.addQuad(0, last_q[0],q[0],q[1],last_q[1])
        this.addQuad(0, last_q[1],q[1],q[2],last_q[2])
        this.addQuad(0, last_q[2],q[2],q[3],last_q[3])
        this.addQuad(0, last_q[3],q[3],q[0],last_q[0])
        last_q = q
      }
      this.addQuad(0, last_q[2],first_q[0],first_q[1],last_q[3])
      this.addQuad(0, last_q[3],first_q[1],first_q[2],last_q[0])
      this.addQuad(0, last_q[0],first_q[2],first_q[3],last_q[1])
      this.addQuad(0, last_q[1],first_q[3],first_q[0],last_q[2])  
    }

    function transform(v:Vector):Vector {
      const [x,y,z] = v
      const vx = x*2.0*PI/l
      const vy = (y-2.5)/5.0
      const a=r*vy*cos(vx/2.0)+h*z*sin(vx/2.0)
      const b=h*z*cos(vx/2.0)-r*vy*sin(vx/2.0)
      return [(R+a)*cos(vx),(R+a)*sin(vx),b]
      }
    
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


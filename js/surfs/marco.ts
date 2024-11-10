import Surf from '../surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin

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

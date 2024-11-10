import Surf from '../surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin

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

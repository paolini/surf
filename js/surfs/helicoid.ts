import Surf from '../surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin

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
    this.addTriangle(0, 0,1,2)
  }

  run() {
    for (let i=0; i<10; i++) {
      this.triangulate()
      this.evolveMeanCurvature(0.05,20)
    }
  }
}

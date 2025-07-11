import Surf from '../surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin

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
    
        this.addTriangle(0, 0, 1, 2)
    }

    run() {
        this.triangulateToR(1)
        this.evolve(0.05,20)
        this.triangulateToR(0.25)
        this.evolve(0.05,20)
    }
}

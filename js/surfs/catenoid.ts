import Surf from '../surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin

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


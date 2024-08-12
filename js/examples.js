import Surf from './surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin

export class Pringle extends Surf {
    constructor(h=0.5,n=2) {
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
    constructor(R=1, h=1) {
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

    run() {
        this.triangulate(1)
        this.evolveMeanCurvature(0.05,20)
        this.triangulate(0.5)
        this.evolveMeanCurvature(0.05,20)
    }
}

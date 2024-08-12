import Surf from './surf'

export class Plateau extends Surf {
    constructor() {
        super()

        this.addBorder(t => 
            ([  Math.cos(2*Math.PI*t), 
                Math.sin(2*Math.PI*t), 
                0.5*Math.sin(4*Math.PI*t)]),
            1.0)

        this.addBorderVertex(0) // v0
        this.addBorderVertex(0.25) // v1
        this.addBorderVertex(0.5) // v2
        this.addBorderVertex(0.75) // v3
    
        this.addTriangle(0, 1, 2)
        this.addTriangle(2, 3, 0)
    }

    run() {
        if (this.has_run) return
        this.has_run = true
        this.triangulate()
        this.evolveMeanCurvature(0.05,5)
        this.triangulate()
        this.evolveMeanCurvature(0.05,5)
        this.triangulate()
        this.evolveMeanCurvature(0.05,5)
        this.triangulate()
        this.evolveMeanCurvature(0.05,5)
    }
}

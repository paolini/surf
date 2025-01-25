import Surf from '../surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin

export class Lens extends Surf {
    constructor() {
      super(`lens`)

      this.addBorder(t => [ 5*cos(t), 5*sin(t), 0 ], 2*PI)

      // 12 vertices cube 
      const v000 = this.addVertex(-.5,-.5,-.5)
      const v001 = this.addVertex(-.5,-.5, .5)
      const v010 = this.addVertex(-.5, .5,-.5)
      const v011 = this.addVertex(-.5, .5, .5)
      const v100 = this.addVertex( .5,-.5,-.5)
      const v101 = this.addVertex( .5,-.5, .5)
      const v110 = this.addVertex( .5, .5,-.5)
      const v111 = this.addVertex( .5, .5, .5)

      const v00 = this.addVertex(-1,-1, 0)
      const v01 = this.addVertex(-1, 1, 0)
      const v10 = this.addVertex( 1,-1, 0)
      const v11 = this.addVertex( 1, 1, 0)

      // 4 vertices square
      const w11 = this.addBorderVertex(1*PI/4)
      const w01 = this.addBorderVertex(3*PI/4)
      const w00 = this.addBorderVertex(5*PI/4)
      const w10 = this.addBorderVertex(7*PI/4)

      this.surfaces = [[],[]]

      // 10 facets cube
      this.addQuad(0,v000,v00 ,v01 ,v010)
      this.addQuad(0,v000,v100,v10 ,v00 )
      this.addQuad(0,v100,v110,v11 ,v10 )
      this.addQuad(0,v010,v01 ,v11 ,v110)

      this.addQuad(0,v00 ,v001,v011,v01 )
      this.addQuad(0,v00 ,v10 ,v101,v001)
      this.addQuad(0,v10 ,v11 ,v111,v101)
      this.addQuad(0,v01 ,v011,v111,v11 )

      this.addQuad(0,v000,v010,v110,v100)
      this.addQuad(0,v001,v101,v111,v011)

      // 4 facets annulus      
      this.addQuad(1,w00,w10,v10,v00)
      this.addQuad(1,w10,w11,v11,v10)
      this.addQuad(1,w11,w01,v01,v11) 
      this.addQuad(1,w01,w00,v00,v01)

      // 1 volume
      this.volumes=[
        {
          surfaces: [[0,1.0]],
          pressure: 1.0
        }
      ]
    }

    run() {}
  }

import Surf from '../surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin

export class Sphere extends Surf {
    constructor() {
      super(`sphere`)

      // 8 vertices cube 
      const v000 = this.addVertex(-.5,-.5,-.5)
      const v001 = this.addVertex(-.5,-.5, .5)
      const v010 = this.addVertex(-.5, .5,-.5)
      const v011 = this.addVertex(-.5, .5, .5)
      const v100 = this.addVertex( .5,-.5,-.5)
      const v101 = this.addVertex( .5,-.5, .5)
      const v110 = this.addVertex( .5, .5,-.5)
      const v111 = this.addVertex( .5, .5, .5)
            
      // 6 facets 
      this.addQuad(0,v000,v001,v011,v010)
      this.addQuad(0,v000,v100,v101,v001)
      this.addQuad(0,v000,v010,v110,v100)
      this.addQuad(0,v100,v110,v111,v101)
      this.addQuad(0,v010,v011,v111,v110)
      this.addQuad(0,v001,v101,v111,v011)

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

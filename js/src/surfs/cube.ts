import Surf from '../surf'

const PI = Math.PI
const cos = Math.cos
const sin = Math.sin

export class Cube extends Surf {
    constructor(simply_connected: boolean = true,sx: number = 1.0,sy: number = 1.0,sz: number = 1.0) {
      super(`cube sx:${sx} sy:${sy} sz:${sz} simply_connected:${simply_connected}`)

      function transform(x:number,y:number,z:number): [number,number,number] {
        return [(x-0.5)*sx, (y-0.5)*sy, (z-0.5)*sz]
      }

      const addVertex = (x,y,z) => {
        const p = transform(x,y,z)
        return this.addVertex(p[0],p[1],p[2])
      }

      // 8 vertices cube 
      const v000 = addVertex(0,0,0)
      const v001 = addVertex(0,0,1)
      const v010 = addVertex(0,1,0)
      const v011 = addVertex(0,1,1)
      const v100 = addVertex(1,0,0)
      const v101 = addVertex(1,0,1)
      const v110 = addVertex(1,1,0)
      const v111 = addVertex(1,1,1)

      // 4 vertices square (m=mid)
      const vm00 = addVertex(0.5,0.3,0.3)
      const vm01 = addVertex(0.5,0.3,0.7)
      const vm10 = addVertex(0.5,0.7,0.3)
      const vm11 = addVertex(0.5,0.7,0.7)

      // 12 borders
      const bx00 = this.addBorder(t => transform(t, 0, 0), 1.0, false)
      const bx01 = this.addBorder(t => transform(t, 0, 1), 1.0, false)
      const bx10 = this.addBorder(t => transform(t, 1, 0), 1.0, false)
      const bx11 = this.addBorder(t => transform(t, 1, 1), 1.0, false)
      const by00 = this.addBorder(t => transform(0, t, 0), 1.0, false)
      const by01 = this.addBorder(t => transform(1, t, 0), 1.0, false)
      const by10 = this.addBorder(t => transform(0, t, 1), 1.0, false)
      const by11 = this.addBorder(t => transform(1, t, 1), 1.0, false)
      const bz00 = this.addBorder(t => transform(0, 0, t), 1.0, false)
      const bz01 = this.addBorder(t => transform(0, 1, t), 1.0, false)
      const bz10 = this.addBorder(t => transform(1, 0, t), 1.0, false)
      const bz11 = this.addBorder(t => transform(1, 1, t), 1.0, false)
      
      // fix border vertices
      this.addBorderVertex(0,bx00,v000)
      this.addBorderVertex(1,bx00,v100)
      this.addBorderVertex(0,bx01,v001)
      this.addBorderVertex(1,bx01,v101)
      this.addBorderVertex(0,bx10,v010)
      this.addBorderVertex(1,bx10,v110)
      this.addBorderVertex(0,bx11,v011)
      this.addBorderVertex(1,bx11,v111)
      this.addBorderVertex(0,by00,v000)
      this.addBorderVertex(1,by00,v010)
      this.addBorderVertex(0,by01,v100)
      this.addBorderVertex(1,by01,v110)
      this.addBorderVertex(0,by10,v001)
      this.addBorderVertex(1,by10,v011)
      this.addBorderVertex(0,by11,v101)
      this.addBorderVertex(1,by11,v111)
      this.addBorderVertex(0,bz00,v000)
      this.addBorderVertex(1,bz00,v001)
      this.addBorderVertex(0,bz01,v010)
      this.addBorderVertex(1,bz01,v011)
      this.addBorderVertex(0,bz10,v100)
      this.addBorderVertex(1,bz10,v101)
      this.addBorderVertex(0,bz11,v110)
      this.addBorderVertex(1,bz11,v111)
      
      // facets 4+4+1 quads, 4 triangles
      this.addQuad(0, v000,vm00,vm10,v010)
      this.addQuad(0, v010,vm10,vm11,v011)
      this.addQuad(0, v011,vm11,vm01,v001)
      this.addQuad(0, v001,vm01,vm00,v000)
      this.addQuad(0, vm00,v100,v110,vm10)
      this.addQuad(0, vm10,v110,v111,vm11)
      this.addQuad(0, vm11,v111,v101,vm01)
      this.addQuad(0, vm01,v101,v100,vm00)
      this.addTriangle(0, v000,v100,vm00)
      this.addTriangle(0, v001,v101,vm01)
      this.addTriangle(0, v010,v110,vm10)
      this.addTriangle(0, v011,v111,vm11)
      if (simply_connected)
        this.addQuad(0, vm00,vm10,vm11,vm01)
    }
  }

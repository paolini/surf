import Surf from '../surf'
import {Vector, squared_norm, vector_diff} from '../algebra'

/***
 *   K. Brakke, Soap films and covering spaces, 1995
 *   There are 5 examples of minimal films over an octahedron, numbered 1-5
 ***/
export class Octa extends Surf {
    mode: string
    constructor(s:string='brakke_1') {
        super(`Octa(${s})`)
        this.mode = s
        function segment(start:Vector,end:Vector):(number)=>Vector {
            const [sx,sy,sz] = start
            const [ex,ey,ez] = end
            return t => [(1-t)*sx+t*ex,(1-t)*sy+t*ey,(1-t)*sz+t*ez]
        }

        const borders = [
            this.addBorder(segment([ 0, 0,-1],[ 0,-1, 0]),1,false),
            this.addBorder(segment([ 0, 0,-1],[ 0, 1, 0]),1,false),
            this.addBorder(segment([ 0, 0,-1],[-1, 0, 0]),1,false),
            this.addBorder(segment([ 0, 0,-1],[ 1, 0, 0]),1,false),
            this.addBorder(segment([ 0, 0, 1],[ 0,-1, 0]),1,false),
            this.addBorder(segment([ 0, 0, 1],[ 0, 1, 0]),1,false),
            this.addBorder(segment([ 0, 0, 1],[-1, 0, 0]),1,false),
            this.addBorder(segment([ 0, 0, 1],[ 1, 0, 0]),1,false),
            this.addBorder(segment([ 0,-1, 0],[-1, 0, 0]),1,false),
            this.addBorder(segment([ 0,-1, 0],[ 1, 0, 0]),1,false),
            this.addBorder(segment([ 0, 1, 0],[-1, 0, 0]),1,false),
            this.addBorder(segment([ 0, 1, 0],[ 1, 0, 0]),1,false)
        ]
            
        const p0=this.addVertex(1,0.,0.)
        const p1=this.addVertex(-1.,0.,0.)
        const p2=this.addVertex(0.,1.,0.)
        const p3=this.addVertex(0.,-1.,0.)
        const p4=this.addVertex(0.,0.,1.)
        const p5=this.addVertex(0.,0.,-1.)

        const p = [p0,p1,p2,p3,p4,p5]

        let count = 0
        for (let i=0;i<this.borders.length;++i) {
            const border = this.borders[i]
            for (let j=0;j<6;j++) {
                const v = this.getVertex(j)
                for (let t=0;t<2;t++) {
                    const w = border.f(t)
                    if (squared_norm(vector_diff(v,w))<0.001) {
                        count ++
                        border.indices.push([j,t])
                    }
                }
            }
        }
        if (count !== 24) throw Error(`invalid count`)

        switch(s) {
            case 'brakke_1': {
                /* la prima di Brakke */
                const q0=this.addVertex(1./3,0.,-1./3)   // (p0+p5)/3
                const q1=this.addVertex(0.,1./3,-1./3)   // (p5+p2)/3
                const q2=this.addVertex(-1./3,1./3,0.)   // (p2+p1)/3
                const q3=this.addVertex(-1./3,0.,1./3)   // (p1+p4)/3
                const q4=this.addVertex(0.,-1./3,1./3)   // (p4+p3)/3
                const q5=this.addVertex(1./3,-1./3,0.)   // (p3+p0)/3
        
                const q6=this.addVertex(0.,0.,0.)   // orig
        
                this.addTriangle(p0, p5, q0)
                this.addTriangle(p5, p2, q1)
                this.addTriangle(p2, p1, q2)
                this.addTriangle(p1, p4, q3)
                this.addTriangle(p4, p3, q4)
                this.addTriangle(p3, p0, q5)
        
                this.addQuad(p0,p2,q1,q0)
                this.addQuad(p2,p4,q3,q2)
                this.addQuad(p4,p0,q5,q4)
        
                this.addQuad(p3,p5,q0,q5)
                this.addQuad(p5,p1,q2,q1)
                this.addQuad(p1,p3,q4,q3)
        
                this.addTriangle(p0, q0, q5)
                this.addTriangle(p2, q2, q1)
                this.addTriangle(p4, q4, q3)
                this.addTriangle(p1, q3, q2)
                this.addTriangle(p3, q5, q4)
                this.addTriangle(p5, q1, q0)
        
                /* esagono centrale */
        
                this.addTriangle(q5, q0, q6)
                this.addTriangle(q0, q1, q6)
                this.addTriangle(q1, q2, q6)
                this.addTriangle(q2, q3, q6)
                this.addTriangle(q3, q4, q6)
                this.addTriangle(q4, q5, q6)
            }
                break
            case 'brakke_2': {
                /* la seconda di Brakke, rispetto a 1 cambiano q0 e q1 */
                const q0=this.addVertex(1./4,1./4,-1./4)   // faccia p[0,2,5]
                const q1=this.addVertex(-1./4,-1./4,-1./4) // faccia p[1,3,5]
                const q2=this.addVertex(-1./3,1./3,0.)   // (p2+p1)/3
                const q3=this.addVertex(-1./3,0.,1./3)   // (p1+p4)/3
                const q4=this.addVertex(0.,-1./3,1./3)   // (p4+p3)/3
                const q5=this.addVertex(1./3,-1./3,0.)   // (p3+p0)/3
        
                const q6=this.addVertex(0.,0.,0.)   // orig
        
                this.addTriangle(p0, p2, q0)
                this.addTriangle(p0, p5, q0)
                this.addTriangle(p5, p2, q0)
                this.addTriangle(p2, p1, q2)
                this.addTriangle(p1, p4, q3)
                this.addTriangle(p4, p3, q4)
                this.addTriangle(p3, p0, q5)
                this.addTriangle(p1, p5, q2)
                this.addTriangle(p3, p5, q5)
        
                this.addQuad(p2,p4,q3,q2)
                this.addQuad(p4,p0,q5,q4)
            
                this.addQuad(p0,q0,q1,q5)
                this.addQuad(p2,q0,q1,q2)
                this.addQuad(p1,p3,q4,q3)
        
                this.addTriangle(p5, q1, q5)
                this.addTriangle(p5, q1, q2)
                this.addTriangle(p4, q4, q3)
                this.addTriangle(p1, q3, q2)
                this.addTriangle(p3, q5, q4)
                this.addTriangle(p5, q1, q0)
        
                /* pentagono centrale */
        
                this.addTriangle(q5, q1, q6)
                this.addTriangle(q1, q2, q6)
                this.addTriangle(q2, q3, q6)
                this.addTriangle(q3, q4, q6)
                this.addTriangle(q4, q5, q6)
            }
            break
        
        case 'brakke_3': /* la terza di Brakke */
          {
            const q0=this.addVertex(1./4,1./4,-1./4)   // (p0+p2+p5)/4
            const q1=this.addVertex(1./4,-1./4,1./4)   // (p0+p3+p4)/4
            const q2=this.addVertex(-1./4,1./4,1./4)   // (p2+p4+p1)/4
            const q3=this.addVertex(-1./4,-1./4.,-1./4)   // (p1+p3+p5)/4
    
            const q6=this.addVertex(0.,0.,0.)   // orig
    
            this.addTriangle(p0, p2, q0)
            this.addTriangle(p0, p5, q0)
            this.addTriangle(p5, p2, q0)
            this.addTriangle(p0, p4, q1)
            this.addTriangle(p0, p3, q1)
            this.addTriangle(p3, p4, q1)
            this.addTriangle(p2, p4, q2)
            this.addTriangle(p1, p2, q2)
            this.addTriangle(p4, p1, q2)
            this.addTriangle(p1, p3, q3)
            this.addTriangle(p3, p5, q3)
            this.addTriangle(p5, p1, q3)
    
            this.addQuad(p0,q0,q6,q1)
            this.addQuad(p2,q2,q6,q0)
            this.addQuad(p4,q1,q6,q2)
            this.addQuad(p1,q2,q6,q3)
            this.addQuad(p3,q1,q6,q3)
            this.addQuad(p5,q0,q6,q3)
          }
        case 'brakke_4': 
        {
            /* la quarta di Brakke */
            const q0=this.addVertex(1./4,1./4,-1./4)   // (p0+p2+p5)/4
            const q1=this.addVertex(1./4,-1./4,1./4)   // (p0+p3+p4)/4
            const q2=this.addVertex(-1./3,1./3,0.)   // (p1+p2)/3
            const q3=this.addVertex(-1./3,0.,1./3)   // (p1+p4)/3
            const q4=this.addVertex(-1./4,-1./4.,-1./3)   // (p1+p3+k*p5)/4
            const q5=this.addVertex(-1./4,-1./3.,-1./4)   // (p1+k*p3+p5)/4
    
            this.addTriangle(p0, p2, q0)
            this.addTriangle(p0, p4, q1)
    
            this.addQuad(p2,p4,q3,q2)
    
            this.addTriangle(p0, p5, q0)
            this.addTriangle(p5, p2, q0)
            this.addTriangle(p0, p3, q1)
            this.addTriangle(p3, p4, q1)
            this.addTriangle(p1, p2, q2)
            this.addTriangle(p1, p4, q3)
    
            this.addTriangle(p1, q3, q2)
    
            this.addQuad(q2,q3,q5,q4)
            this.addQuad(p2,q2,q4,q0)
            this.addQuad(p4,q1,q5,q3)
    
            this.addTriangle(p0, q0, q4)
            this.addTriangle(p0, q4, q5)
            this.addTriangle(p0, q5, q1)
    
            this.addTriangle(p5, q0, q4)
            this.addTriangle(p3, q1, q5)
            this.addQuad(p3,p5,q4,q5)
    
            this.addQuad(p1,q2,q4,p5)
            this.addQuad(p1,q3,q5,p3)
          }
          break
        case 'brakke_5': {
            /* la quinta di Brakke */
            const q0=this.addVertex(1./3,-1./3,0.)   // (p0+p3)/3
            const q1=this.addVertex(-1./3,1./3,0.)   // (p1+p2)/3
            const q2=this.addVertex(1./4,1./4,1./4)  // (p0+p2+p4)/4
            const q3=this.addVertex(0.,0.,1./3)      // (2*p4+p5)/3
            const q4=this.addVertex(-1./4,-1./4,-1./4)  // (p1+p3+p5)/4
            const q5=this.addVertex(0.,0.,-1./3)     // (2*p5+p4)/3
    
            this.addTriangle(p0, p4, q2)
            this.addTriangle(p4, p2, q2)
            this.addTriangle(p0, p2, q2)
            this.addTriangle(p3, p5, q4)
            this.addTriangle(p3, p1, q4)
            this.addTriangle(p1, p5, q4)
            this.addTriangle(p0, p3, q0)
            this.addTriangle(p1, p2, q1)
    
            this.addQuad(p0,p5,q5,q0)
            this.addQuad(p2,p5,q5,q1)
            this.addQuad(p1,p4,q3,q1)
            this.addQuad(p3,p4,q3,q0)
    
            this.addTriangle(p4, q2, q3)
            this.addTriangle(p5, q4, q5)
    
            this.addQuad(p0,q2,q3,q0)
            this.addQuad(p3,q0,q5,q4)
            this.addQuad(p1,q4,q5,q1)
            this.addQuad(p2,q1,q3,q2)
        
            this.addQuad(q0,q3,q1,q5)
            }
            break
        case 'brakke_a': {
            /* facce a scacchiera */
            this.addTriangle(p0, p2, p4)
            this.addTriangle(p0, p3, p5)
            this.addTriangle(p1, p2, p5)
            this.addTriangle(p1, p3, p4)
          }
          break
        case 'brakke_b': {
            /* tre facce aperte */
            const q0=this.addVertex(1./3,1./3,0.)   // (p0+p2)/3 = aletta02
            const q1=this.addVertex(0.,1./3,1./3)   // aletta24
            const q2=this.addVertex(1./3,0.,1./3)   // aletta40
        
            /* alette */
        
            this.addTriangle(p0, p2, q0)
            this.addTriangle(p2, p4, q1)
            this.addTriangle(p4, p0, q2)
        
            /* top triangle */
        
            this.addTriangle(p0, q0, q2)
            this.addTriangle(p2, q1, q0)
            this.addTriangle(p4, q2, q1)
            this.addTriangle(q0, q1, q2)
        
            /* positive trias */
        
            this.addTriangle(p5, p2, q0)
            this.addTriangle(p5, q0, p0)
            this.addTriangle(p1, p4, q1)
            this.addTriangle(p1, q1, p2)
            this.addTriangle(p3, p0, q2)
            this.addTriangle(p3, q2, p4)
        
            /* bottom triangle */
        
            this.addTriangle(p1, p3, p5)
          }
        break
        case 'brakke_c': {
            /* fascia laterale */
            const q0=this.addVertex(1./3,-1./3,0.)   // (p0+p3)/3 = aletta03
            const q1=this.addVertex(1./3,0.,-1./3)   // aletta05
            const q2=this.addVertex(0.,1./3,-1./3)   // aletta25
            const q3=this.addVertex(-1./3,1./3,0.)   // aletta21
            const q4=this.addVertex(-1./3,0.,1./3)   // aletta41
            const q5=this.addVertex(0.,-1./3,1./3)   // aletta43
        
            /* alette */
        
            this.addTriangle(p0, p3, q0)
            this.addTriangle(p0, p5, q1)
            this.addTriangle(p2, p5, q2)
            this.addTriangle(p2, p1, q3)
            this.addTriangle(p4, p1, q4)
            this.addTriangle(p4, p3, q5)
        
            /* positive trias */
        
            this.addTriangle(p0, q0, q1)
            this.addTriangle(p2, q2, q3)
            this.addTriangle(p4, q4, q5)
        
            /* negative trias */
        
            this.addTriangle(p1, q3, q4)
            this.addTriangle(p3, q5, q0)
            this.addTriangle(p5, q1, q2)
        
            /* negative trapezoids */
        
            this.addQuad(p3,p5,q1,q0)
            this.addQuad(p5,p1,q3,q2)
            this.addQuad(p1,p3,q5,q4)
        
            /* positive trapezoids */
        
            this.addQuad(p4,p2,q3,q4)
            this.addQuad(p0,p4,q5,q0)
            this.addQuad(p2,p0,q1,q2)
          }
          break
        case 'brakke_d': {
            /* superficie skew */
            const q0=this.addVertex(0.,0.5,0.)
            const q1=this.addVertex(0.,-0.5,0.)
            const q2=this.addVertex(0.,0.,0.5)
            const q3=this.addVertex(0.,0.,-0.5)
            this.addTriangle(p0, p2, q0)
            this.addTriangle(p0, p4, q2)
            this.addTriangle(p0, q0, q2)
        
            this.addTriangle(p0, p3, q1)
            this.addTriangle(p0, p5, q3)
            this.addTriangle(p0, q1, q3)
        
            this.addTriangle(p1, p2, q0)
            this.addTriangle(p1, p5, q3)
            this.addTriangle(p1, q0, q3)
        
            this.addTriangle(p1, p3, q1)
            this.addTriangle(p1, p4, q2)
            this.addTriangle(p1, q1, q2)
        
            this.addQuad(p2,p4,q2,q0)
            this.addQuad(p4,p3,q1,q2)
            this.addQuad(p3,p5,q3,q1)
            this.addQuad(p5,p2,q0,q3)
        
            this.addQuad(q0,q2,q1,q3)
        }
      }
    }
}

import * as THREE from 'three'

const scene = new THREE.Scene()
const camera = new THREE.PerspectiveCamera( 75, window.innerWidth / window.innerHeight, 0.1, 1000 )

const renderer = new THREE.WebGLRenderer()
renderer.setSize( window.innerWidth, window.innerHeight )
document.body.appendChild( renderer.domElement )

class Mesh {
    constructor() {
        this.buffer = new ArrayBuffer(0)
        this.vertices = Float32Array.from(0)
        this.indices = []
    }

    resizeVertices(n) {
        const bufferSize = this.buffer.byteLength
        const requiredSize = 4*3*n
        if (bufferSize < requiredSize) {     
            const newBufferSize = Math.max(2*bufferSize,requiredSize)
            console.log("reallocating", newBufferSize)
            this.buffer = new ArrayBuffer(newBufferSize)
            const newVertices = new Float32Array(this.buffer, 0, 3*n)
            newVertices.set(this.vertices)
            this.vertices = newVertices
        } else {
            this.vertices = new Float32Array(this.buffer, 0, 3*n)
        }
    }

    addVertex(x, y, z) {
        const n = this.vertices.length / 3
        this.resizeVertices(n+1)
        this.vertices[3*n] = x
        this.vertices[3*n + 1] = y
        this.vertices[3*n + 2] = z
        return n
    }

    addTriangle(a, b, c) {
        this.indices.push(a, b, c)
    }
}

const myMesh = new Mesh()

myMesh.addVertex(-1.0, -1.0,  1.0) // v0
myMesh.addVertex( 1.0, -1.0,  1.0) // v1
myMesh.addVertex( 1.0,  1.0,  1.0) // v2
myMesh.addVertex(-1.0,  1.0,  1.0) // v3

myMesh.addTriangle(0, 1, 2)
myMesh.addTriangle(2, 3, 0)

const vertices = new Float32Array( [
	-1.0, -1.0,  1.0, // v0
	 1.0, -1.0,  1.0, // v1
	 1.0,  1.0,  1.0, // v2
	-1.0,  1.0,  1.0, // v3
] )

const indices = [
	0, 1, 2,
	2, 3, 0,
]

const geometry = new THREE.BufferGeometry()
geometry.setIndex( myMesh.indices )
geometry.setAttribute( 'position', new THREE.BufferAttribute( myMesh.vertices, 3 ) )
const material = new THREE.MeshBasicMaterial( { color: 0xff0000 } )

// const geometry = new THREE.BoxGeometry( 1, 1, 1 )
// const material = new THREE.MeshBasicMaterial( { color: 0x00ff00 } )
material.wireframe = true
const mesh = new THREE.Mesh( geometry, material )
scene.add( mesh )

camera.position.z = 5

function animate() {
	requestAnimationFrame( animate )
    mesh.rotation.x += 0.01
    mesh.rotation.y += 0.01
	renderer.render( scene, camera )
}
animate()

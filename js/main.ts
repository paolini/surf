import * as THREE from 'three'

import Mesh from './mesh'

const scene = new THREE.Scene()
const camera = new THREE.PerspectiveCamera( 75, window.innerWidth / window.innerHeight, 0.1, 1000 )

const renderer = new THREE.WebGLRenderer()
renderer.setSize( window.innerWidth, window.innerHeight )
document.body.appendChild( renderer.domElement )

const myMesh = new Mesh()

myMesh.addBorder(t => 
    ([  Math.cos(2*Math.PI*t), 
        Math.sin(2*Math.PI*t), 
        Math.sin(2*t)]),
    1.0)

myMesh.addBorderVertex(0) // v0
myMesh.addBorderVertex(0.25) // v1
myMesh.addBorderVertex(0.5) // v2
myMesh.addBorderVertex(0.75) // v3

myMesh.addTriangle(0, 1, 2)
myMesh.addTriangle(2, 3, 0)

myMesh.triangulate()
myMesh.triangulate()

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

const material = new THREE.MeshBasicMaterial( { color: 0xff0000 } )

// const geometry = new THREE.BoxGeometry( 1, 1, 1 )
// const material = new THREE.MeshBasicMaterial( { color: 0x00ff00 } )
material.wireframe = true

function generateThreeMesh(myMesh: Mesh, material: THREE.Material) {
	const geometry = new THREE.BufferGeometry()
	geometry.setIndex( myMesh.indices )
	geometry.setAttribute( 'position', new THREE.BufferAttribute( myMesh.vertices, 3 ) )
	return new THREE.Mesh( geometry, material )
}

/*
const geometry = new THREE.BufferGeometry()
geometry.setIndex( myMesh.indices )
geometry.setAttribute( 'position', new THREE.BufferAttribute( myMesh.vertices, 3 ) )
let mesh = new THREE.Mesh( geometry, material )
*/
let mesh = generateThreeMesh(myMesh, material)
scene.add( mesh )

camera.position.z = 5

function animate() {
	requestAnimationFrame( animate )
    mesh.rotation.x += 0.01
    mesh.rotation.y += 0.01
	renderer.render( scene, camera )
}
animate()

// movement - please calibrate these values
var xSpeed = 0.0001;
var ySpeed = 0.0001;

document.addEventListener("keydown", onDocumentKeyDown, false);

function onDocumentKeyDown(event) {
	var key= event.key
    if (key == 't') {
		console.log(`triangulating`)
		scene.remove(mesh)
		myMesh.triangulate()
		mesh = generateThreeMesh(myMesh, material)
		scene.add(mesh)
    } else {
		console.log(`unknown command. key: ${event.key} keyCode: ${event.which}`)
	}
};
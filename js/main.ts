import * as THREE from 'three'
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js';

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
        0.5*Math.sin(4*Math.PI*t)]),
    1.0)

myMesh.addBorderVertex(0) // v0
myMesh.addBorderVertex(0.25) // v1
myMesh.addBorderVertex(0.5) // v2
myMesh.addBorderVertex(0.75) // v3

myMesh.addTriangle(0, 1, 2)
myMesh.addTriangle(2, 3, 0)

myMesh.triangulate()
myMesh.triangulate()

const material = new THREE.MeshBasicMaterial( { color: 0xff0000 } )
material.wireframe = true

function generateThreeMesh(myMesh: Mesh, material: THREE.Material) {
	const geometry = new THREE.BufferGeometry()
	geometry.setIndex( myMesh.indices )
	geometry.setAttribute( 'position', new THREE.BufferAttribute( myMesh.vertices, 3 ) )
	return new THREE.Mesh( geometry, material )
}


let mesh = generateThreeMesh(myMesh, material)
scene.add( mesh );

camera.position.z = 5;

const controls = new OrbitControls(camera, renderer.domElement);
controls.enableDamping = true; // an animation loop is required when either damping or auto-rotation are enabled
controls.dampingFactor = 0.25;
controls.screenSpacePanning = false;
controls.minDistance = 1;
controls.maxDistance = 1000;
controls.maxPolarAngle = Math.PI / 2;

function animate() {
	requestAnimationFrame( animate )
	controls.update(); // only required if controls.enableDamping = true, or if controls.autoRotate = true
	renderer.render(scene, camera);
}
animate()

document.addEventListener("keydown", onDocumentKeyDown, false);

function onDocumentKeyDown(event) {
	var key= event.key
    if (key == 't') {
		console.log(`triangulating`)
		scene.remove(mesh)
		myMesh.triangulate()
		mesh = generateThreeMesh(myMesh, material)
		scene.add(mesh)
	} else if (key == 'e') {
		console.log(`evolving`)
		const meanCurvatureVector = myMesh.computeMeanCurvatureVector()
		myMesh.evolveMeanCurvatureVector(0.01, meanCurvatureVector)
		mesh.geometry.attributes.position.needsUpdate = true
    } else {
		console.log(`unknown command. key: ${event.key} keyCode: ${event.which}`)
	}
};
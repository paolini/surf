import * as THREE from 'three'
import {OrbitControls} from 'three/examples/jsm/controls/OrbitControls.js'

import {Plateau} from './examples'

const scene = new THREE.Scene()
const camera = new THREE.PerspectiveCamera(75, window.innerWidth/window.innerHeight, 0.1, 1000)
camera.position.x = 5;
camera.up.set(0, 0, 1);

const renderer = new THREE.WebGLRenderer()
renderer.setSize(window.innerWidth, window.innerHeight)
document.body.appendChild(renderer.domElement)

const surf = new Plateau()

// Create a material
const material = new THREE.MeshPhongMaterial();
// material.wireframe = true
material.side = THREE.DoubleSide;
material.transparent = true;
material.opacity = 0.5;
material.flatShading = true;
material.emissive = new THREE.Color("blue");
material.emissiveIntensity = 4

const color = 0xFFFFFF;
const intensity = 1;
const light = new THREE.DirectionalLight(color, intensity);
//light.position.set(0, 10, 0);
//light.target.position.set(-5, 0, 0);
scene.add(light);
scene.add(light.target);

function generateThreeMesh(myMesh: Surf, material: THREE.Material) {
	const geometry = new THREE.BufferGeometry()
	geometry.setIndex( myMesh.indices )
	geometry.setAttribute( 'position', new THREE.BufferAttribute( myMesh.vertices, 3 ) )
	return new THREE.Mesh( geometry, material )
}


let mesh = generateThreeMesh(surf, material)
scene.add( mesh );

const controls = new OrbitControls(camera, renderer.domElement);
controls.enableDamping = true; // an animation loop is required when either damping or auto-rotation are enabled
controls.dampingFactor = 0.5;
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

function triangulate() {
	scene.remove(mesh)
	surf.triangulate()
	mesh = generateThreeMesh(surf, material)
	scene.add(mesh)
}

function evolve(count=1) {
	surf.evolveMeanCurvature(0.05,count)
	mesh.geometry.attributes.position.needsUpdate = true
}

function run() {
	scene.remove(mesh)
	surf.run()
	mesh = generateThreeMesh(surf, material)
	scene.add(mesh)
	mesh.geometry.attributes.position.needsUpdate = true
}

if (true) run()

document.addEventListener("keydown", onDocumentKeyDown, false);

function onDocumentKeyDown(event) {
	var key= event.key
    if (key == 't') {
		console.log(`triangulating`)
		triangulate()
	} else if (key == 'e') {
		console.log(`evolving`)
		evolve()
	} else if (key == 'r') {
		console.log('run')
		run()
	} else if (key == 'w') {
		console.log(`wireframe`)
		material.wireframe = !material.wireframe
    } else {
		console.log(`unknown command. key: ${event.key} keyCode: ${event.which}`)
	}
}
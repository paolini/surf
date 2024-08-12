import * as THREE from 'three'
import {OrbitControls} from 'three/examples/jsm/controls/OrbitControls.js'

import Surf from './surf'
import {Pringle,Catenoid,Cube,Helicoid,Manu} from './examples'

class SurfMesh extends THREE.Mesh {
	surf: Surf

	constructor(surf: Surf, material: THREE.Material) {
		const geometry = new THREE.BufferGeometry()
		geometry.setIndex( surf.indices )
		geometry.setAttribute( 'position', new THREE.BufferAttribute( surf.vertices, 3 ) )
		super( geometry, material )
	}
} 

class World {
	AUTO_RUN: boolean
	surfMesh: SurfMesh|undefined
	scene: THREE.Scene
	camera: THREE.Camera
	renderer: THREE.Renderer
	material: THREE.Material
	controls: THREE.Controls

	constructor() {
		this.AUTO_RUN = true

		this.scene = new THREE.Scene()
		this.camera = new THREE.PerspectiveCamera(75, window.innerWidth/window.innerHeight, 0.1, 1000)
		this.camera.position.x = 5;
		this.camera.up.set(0, 0, 1);

		this.renderer = new THREE.WebGLRenderer()
		this.renderer.setSize(window.innerWidth, window.innerHeight)
		document.body.appendChild(this.renderer.domElement)

		this.material = new THREE.MeshPhongMaterial()
		// this.material.wireframe = true
		this.material.side = THREE.DoubleSide;
		this.material.transparent = true;
		this.material.opacity = 0.5;
		this.material.flatShading = true;
		this.material.emissive = new THREE.Color("blue");
		this.material.emissiveIntensity = 4

		const color = 0xFFFFFF;
		const intensity = 1;
		const light = new THREE.DirectionalLight(color, intensity);
		light.position.set(0, 10, 0);
		light.target.position.set(-5, 0, 0);
		this.scene.add(light);
		this.scene.add(light.target);

		this.controls = new OrbitControls(this.camera, this.renderer.domElement)
		this.controls.enableDamping = true; // an animation loop is required when either damping or auto-rotation are enabled
		this.controls.dampingFactor = 0.5;
		this.controls.screenSpacePanning = false;
		this.controls.minDistance = 1;
		this.controls.maxDistance = 1000;
		this.controls.maxPolarAngle = Math.PI / 2;
		
		this.load(Pringle)
		document.addEventListener("keydown", evt => this.onDocumentKeyDown(evt), false);

		this.animate()
	}

	animate() {
		requestAnimationFrame(() => this.animate())
		this.controls.update()
		this.renderer.render(this.scene, this.camera)
	}
	
	replaceSurf(surf: Surf) {
		if (this.surfMesh) this.scene.remove(this.surfMesh)
		this.surfMesh = new SurfMesh(surf, this.material)
		this.scene.add(this.surfMesh)	
	}

	load(SomeSurf) {
		console.log(`loading ${SomeSurf.name}`)
		const surf = new SomeSurf()
		if (this.AUTO_RUN) surf.run()
		this.replaceSurf(surf)
	}

	triangulate() {
		if (!this.surfMesh) return
		const surf = this.surfMesh.surf
		surf.triangulate()
		this.replaceSurf(surf)
	}

	evolve(count=1) {
		if (!this.surfMesh) return
		this.surfMesh.surf.evolveMeanCurvature(0.05,count)
		this.surfMesh.geometry.attributes.position.needsUpdate = true
	}

	run() {
		if (!this.surfMesh) return
		const surf = this.surfMesh.surf
		surf.run()
		this.replaceSurf(surf)
	}	

	onDocumentKeyDown(event) {
		var key= event.key
		switch (key) {
			case '0':
				this.load(Pringle)
				break
			case '1':
				this.load(Catenoid)
				break
			case '2':
				this.load(Cube)
				break
			case '3':
				this.load(Helicoid)
				break
			case 'M':
				this.load(Manu)
				break
			case 't':
				console.log(`triangulating`)
				this.triangulate()
				break
			case 'e':
				console.log(`evolving`)
				this.evolve()
				break
			case 'r':
				console.log('run')
				this.run()
				break
			case 'w': 
				console.log(`wireframe`)
				this.material.wireframe = !this.material.wireframe
				break
			default:
				console.log(`unknown command. key: ${event.key} keyCode: ${event.which}`)
		}
	}
}

const world = new World()


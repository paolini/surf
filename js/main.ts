import * as THREE from 'three'
import {OrbitControls} from 'three/examples/jsm/controls/OrbitControls.js'

import Surf from './surf'
import {Pringle} from './surfs/pringle'
import {Catenoid} from './surfs/catenoid'
import {Cube} from './surfs/cube'
import {Helicoid} from './surfs/helicoid'
import {Manu} from './surfs/manu'
import {Marco} from './surfs/marco'
import {Moebius} from './surfs/moebius'
import {MoebiusOriented} from './surfs/moebius'
import {Octa} from './surfs/octa'

class SurfMesh extends THREE.Mesh {
	surf: Surf

	constructor(surf: Surf, material: THREE.Material) {
		const geometry = new THREE.BufferGeometry()
		geometry.setIndex( surf.indices )
		geometry.setAttribute( 'position', new THREE.BufferAttribute( surf.vertices, 3 ) )
		super( geometry, material )
		this.surf = surf
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

		// Get the existing canvas element
		const canvas = document.getElementById('my-canvas') as HTMLCanvasElement;
		
		// Create the renderer using the existing canvas
		this.renderer = new THREE.WebGLRenderer({ canvas });
		this.renderer.setSize(canvas.width, canvas.height);		
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
		this.controls.addEventListener('end', () => this.updateInfo())
		
		this.load(new Pringle())
		this.updateInfo()
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

	load(surf: Surf) {
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

	updateInfo() {
		const info: any = {}

		const camera = this.camera as THREE.PerspectiveCamera
		if (camera) {
			info.camera = {
				position: camera.position.toArray(),
				rotation: camera.rotation.toArray(),
				fov: camera.fov,
				near: camera.near,
				far: camera.far,
			}
		}

		const surf = this.surfMesh?.surf
		if (surf) {
			info.surf = {
				name: surf.name,
				vertices: surf.vertices.length,
				faces: surf.indices.length/3,
				area: surf.computeArea(),
			}
		}

		const info_div = document.getElementById('my-info')
		if (info_div) {
			info_div.textContent = JSON.stringify(info)
		}
	}

	onDocumentKeyDown(event) {
		var key= event.key
		switch (key) {
			case ' ':
				// use to update info
				break
			case '0':
				this.load(new Pringle())
				break
			case '1':
				this.load(new Catenoid())
				break
			case '2':
				this.load(new Cube())
				break
			case '3':
				this.load(new Helicoid())
				break
			case '4':
				this.load(new Marco())
				break
			case '5':
				this.load(new Moebius())
				break
			case '6':
				this.load(new MoebiusOriented())
				break
			case '8': {
					const modes = ['brakke_1','brakke_2','brakke_3','brakke_4','brakke_5','brakke_a','brakke_b','brakke_c','brakke_d']
					const mode = modes[((modes.indexOf(this.surfMesh?.surf?.mode) ?? -1)+1) % modes.length]
					console.log(`loading Octa(${mode})`)
					this.load(new Octa(mode))
				}
				break
			case 'M':
				this.load(new Manu())
				break
			case 't':
				console.log(`triangulating`)
				this.triangulate()
				break
			case 'e':
				console.log(`evolving`)
				this.evolve(0.05)
				break
			case 'r':
				console.log('run')
				this.run()
				break
			case 'w': 
				console.log(`wireframe`)
				this.material.wireframe = !this.material.wireframe
				break
			case 'S':
				console.log('export STL')
				const stl = this.surfMesh?.surf.exportToSTL()
				if (stl) download(stl, 'surf.stl', 'text/plain')
				break
			case 'O':
				console.log('export POV-RAY')
				const pov = this.surfMesh?.surf.exportToPovRay(this.camera)
				if (pov) download(pov, 'surf.pov', 'text/plain')
				break
			default:
				console.log(`unknown command. key: ${event.key} keyCode: ${event.which}`)
		}
		this.updateInfo()
	}
}

function download(data: string, filename: string, type: string) {
	const file = new Blob([data], {type: type})
	const url = URL.createObjectURL(file)
	const a = document.createElement("a")
	a.href = url
	a.download = filename
	a.click()
	URL.revokeObjectURL(url)
}

const world = new World()


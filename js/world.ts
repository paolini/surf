import * as THREE from 'three'
import {OrbitControls} from 'three/examples/jsm/controls/OrbitControls.js'

import Surf from './surf'

class SurfMesh extends THREE.Mesh {
    //geometry: any // workaround: THREE.Mesh should declare this!

	constructor(surf: Surf, triangles, material: THREE.MeshBasicMaterial) {
		const geometry = new THREE.BufferGeometry()
		geometry.setIndex( triangles )
		geometry.setAttribute( 'position', new THREE.BufferAttribute( surf.vertices, 3 ) )
		geometry.computeVertexNormals()
		super( geometry, material )
	}
} 

class SurfObject extends THREE.Group {
	surf: Surf
	material: THREE.MeshBasicMaterial

	constructor(surf: Surf, material: THREE.MeshBasicMaterial) {
		super()
		this.material = material
		this.attachSurf(surf)
	}

	reload(surf: Surf) {
		this.clear()
		this.surf = surf
		const self = this
		surf.surfaces.map(function(triangles){
			self.add(new SurfMesh(surf, triangles, material))
		})
	}
}

export default class World {
    $info: HTMLElement|null 
    $title: HTMLElement|null
	AUTO_RUN: boolean
	surfObjects: SurfObject[]
	scene: THREE.Scene
	camera: THREE.Camera
	renderer: THREE.Renderer
	material: THREE.Material
	controls: THREE.Controls

	constructor(options: {$canvas: HTMLCanvasElement, $title?: HTMLElement, $info?: HTMLElement}) {
        this.$info = options.$info || null
        this.$title = options.$title || null
        const canvas = options.$canvas
		this.AUTO_RUN = true

		this.scene = new THREE.Scene()
		this.camera = new THREE.PerspectiveCamera(75, window.innerWidth/window.innerHeight, 0.1, 1000)
		this.camera.position.x = 5;
		this.camera.up.set(0, 0, 1);
		
        
		// Create the renderer using the given canvas
		this.renderer = new THREE.WebGLRenderer({ canvas });
		this.renderer.setSize(canvas.width, canvas.height);		
		this.material = new THREE.MeshPhongMaterial()
		// this.material.wireframe = true
		this.material.side = THREE.DoubleSide;
		this.material.color.setRGB(0,0.5,1);
		this.material.transparent = true;
		this.material.opacity = 0.85;
		if (false) {
			this.material.flatShading = true;
			this.material.emissive = new THREE.Color("blue");
			this.material.emissiveIntensity = 4
		}

		if (true) {
			const color = 0xFFFFFF;
			const intensity = 1;
			const light = new THREE.DirectionalLight(color, intensity);
			light.position.set(0, 10, 0);
			light.target.position.set(-5, 0, 0);
			this.scene.add(light);
			this.scene.add(light.target);
		}

		if (true) {
			const sunlight = new THREE.HemisphereLight( 0xffffbb, 0x080820, 1 );
			this.scene.add(sunlight)
		}


		this.controls = new OrbitControls(this.camera, this.renderer.domElement)
		this.controls.enableDamping = true; // an animation loop is required when either damping or auto-rotation are enabled
		this.controls.dampingFactor = 0.5;
		this.controls.screenSpacePanning = false;
		this.controls.minDistance = 1;
		this.controls.maxDistance = 1000;
		this.controls.maxPolarAngle = Math.PI / 2;
		this.controls.addEventListener('end', () => this.updateInfo())
		
		this.updateInfo()
		this.animate()
	}

	animate() {
		requestAnimationFrame(() => this.animate())
		this.controls.update()
		this.renderer.render(this.scene, this.camera)
	}
	
	replaceSurf(surf: Surf) {
		const scene = this.scene
		this.surfObjects.forEach(function(surfObject) {
			surfObject.remove_from_scene(scene)
		})
		this.surfObjects
		this.surfMeshes = surf.surfaces.map(function(_, surf_n) {
			const mesh = new SurfMesh(surf, surf_n, this.material)
			this.scene.add(mesh)
			return mesh
		})
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

		if (this.$info) {
			this.$info.textContent = JSON.stringify(info,null,2)
		}

        if (this.$title) {
            this.$title.textContent = `Surf [${surf?.name || 'No Surface'}]`
        }
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

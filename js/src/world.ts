import * as THREE from 'three'
import {OrbitControls} from 'three/examples/jsm/controls/OrbitControls.js'

import Surf from './surf'

class SurfMesh extends THREE.Mesh {
	constructor(surf: Surf, surf_number: number, material: THREE.Material) {
		const triangles = surf.surfaces[surf_number]
		const geometry = new THREE.BufferGeometry()
		geometry.setIndex( triangles )
		geometry.setAttribute('position', new THREE.BufferAttribute( surf.vertices, 3 ) )
		// geometry.computeVertexNormals()
		super( geometry, material )
		if (!surf.surfacesNormalAreaVectors) {
			surf.computeMeanCurvatureVector()
			if (!surf.surfacesNormalAreaVectors) {
				console.log(`ERROR: normalAreaVectors are not being computed`)
				return
			}
		}
		const normalAreaVectors = surf.surfacesNormalAreaVectors[surf_number]
		const normals = new Float32Array(normalAreaVectors)
		const normalAttribute = new THREE.BufferAttribute( normals, 3)
		geometry.setAttribute('normal', normalAttribute)
		geometry.normalizeNormals()
		normalAttribute.needsUpdate = true
	}
} 

class SurfObject extends THREE.Group {
	surf: Surf
	material: THREE.Material

	constructor(surf: Surf, material: THREE.Material) {
		super()
		this.material = material
		this.surf = surf
		this.reload()
	}

	reload() {
		this.clear()
		const self = this
		for (let surf_number = 0; surf_number < this.surf.surfaces.length; surf_number ++) {
			self.add(new SurfMesh(self.surf, surf_number, self.material))
		}
	}
}

export default class World {
    $info: HTMLElement|null 
    $title: HTMLElement|null
	AUTO_RUN: boolean
	surfObject: SurfObject|null
	scene: THREE.Scene
	camera: THREE.Camera
	renderer: THREE.WebGLRenderer
	material: THREE.MeshPhongMaterial
	controls: OrbitControls

	constructor(options: {$canvas: HTMLCanvasElement, $title?: HTMLElement, $info?: HTMLElement}) {
        this.$info = options.$info || null
        this.$title = options.$title || null
        const canvas = options.$canvas
		this.AUTO_RUN = true

		this.scene = new THREE.Scene()
		this.camera = new THREE.PerspectiveCamera(75, window.innerWidth/window.innerHeight, 0.1, 1000)
		this.camera.position.x = 2;
		this.camera.up.set(0, 0, 1);
		
        
		// Create the renderer using the given canvas
		this.renderer = new THREE.WebGLRenderer({ canvas });
		this.renderer.setSize(canvas.width, canvas.height);		
		this.material = new THREE.MeshPhongMaterial()
		// this.material.wireframe = true
		this.material.side = THREE.DoubleSide
		this.material.color.setRGB(0,0.5,1);
		this.material.transparent = true;
		this.material.opacity = 0.85;
		if (false) {
			this.material.flatShading = true;
			this.material.emissive = new THREE.Color("blue");
			this.material.emissiveIntensity = 4
		}

		if (true) {
			const color = 0xFFFF00;
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
	
	load(surf: Surf) {
		if (this.surfObject) this.scene.remove(this.surfObject)
		if (this.AUTO_RUN) surf.run()
		this.surfObject = new SurfObject(surf, this.material)
		this.scene.add(this.surfObject)
	}

	triangulate() {
		if (!this.surfObject) return
		this.surfObject.surf.triangulateToR()
		this.surfObject.reload()
	}

	evolve(count=1) {
		if (!this.surfObject) return
		this.surfObject.surf.evolve(0.05,count)
		this.surfObject.children.forEach(mesh => (mesh as SurfMesh).geometry.attributes.position.needsUpdate = true)
	}

	equalize() {
		if (!this.surfObject) return
		this.surfObject.surf.equalizeTriangles()
		this.surfObject.reload()
	}

	run() {
		if (!this.surfObject) return
		this.surfObject.surf.run()
		this.surfObject.reload()
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

		if (this.surfObject) {
			const surf = this.surfObject.surf
			info.surf = {
				name: surf.name,
				vertices: surf.vertices.length,
				faces: surf.surfaces.map(indices => indices.length/3),
				area: surf.surfaces.map((indices,i) => surf.computeArea(i)),
//				areas: surf.vertexAreas,
//				areaVectors: surf.surfacesNormalAreaVectors,
				volumes: surf.volumes,
//				pressures: surf.surfacesPressures,
			}
		}

		if (this.$info) {
			this.$info.textContent = JSON.stringify(info,null,2)
		}

        if (this.$title) {
            this.$title.textContent = `Surf [${this.surfObject?.surf.name || 'No Surface'}]`
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

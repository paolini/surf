import * as THREE from 'three'
import {writeFileSync} from 'fs'

import {Pringle} from './surfs/pringle'

const surf = new Pringle()
const camera = new THREE.PerspectiveCamera()
surf.run()
const out = surf.exportToPovRay(camera)
// write out to file
console.log(`writing to pringle.pov`)
writeFileSync('pringle.pov', out)


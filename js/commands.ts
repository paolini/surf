import World from './world'

import {Pringle} from './surfs/pringle'
import {Catenoid} from './surfs/catenoid'
import {Cube} from './surfs/cube'
import {Helicoid} from './surfs/helicoid'
import {Manu} from './surfs/manu'
import {Marco} from './surfs/marco'
import {Moebius} from './surfs/moebius'
import {MoebiusOriented} from './surfs/moebius'
import {Octa} from './surfs/octa'
import {Sphere} from './surfs/sphere'

class Command {
    action: (World) => void
    description: string

    constructor(action: (World) => void, description='') {
        this.action = action
        this.description = description
    }

    exec(world: World) {
        this.action(world)
    }
}

export default class Commands {
    commands: { [key: string]: Command }
    octa_mode: number

    constructor() {
        this.octa_mode = -1
        this.commands = {
            '0': new Command(world => 
                world.load(new Pringle()), 
                'pringle'),
            '1': new Command(world => 
                world.load(new Catenoid()), 
                'catenoid'),
            '2': new Command(world => 
                world.load(new Cube()), 
                'cube'),
            '3': new Command(world => 
                world.load(new Helicoid()), 
                'helicoid'),
            '4': new Command(world => 
                world.load(new Marco()), 
                'Costa\'s'),
            '5': new Command(world => 
                world.load(new Moebius()), 
                'moebius'),
            '6': new Command(world => 
                world.load(new MoebiusOriented()), 
                'moebius oriented'),
            '8': new Command(world =>
                this.loadNextOctaMode(world), 
                'cycle octahedrons'),
            '9': new Command(world =>
                world.load(new Sphere()), 
                'sphere'),
            'M': new Command(world => 
                world.load(new Manu()), 
                'manu'),
            't': new Command(world => 
                world.triangulate(), 
                'triangulate'),
            'e': new Command(world => 
                world.evolve(0.05), 
                'evolve'),
            'r': new Command(world => 
                world.run(), 
                'run'),
            'w': new Command(world => {
                    world.material.wireframe = !world.material.wireframe
                }, 'wireframe'),
            'S': new Command(world => {
                    const stl = world.surfMesh?.surf.exportToSTL()
                    if (stl) download(stl, 'surf.stl', 'text/plain')
                }, 'export STL'),
            'O': new Command(world => {
                    const pov = world.surfMesh?.surf.exportToPovRay(world.camera)
                    if (pov) download(pov, 'surf.pov', 'text/plain')
                }, 'export POV-RAY')
        }
    }
        
    exec(key: string, world: World) {
        const command = this.commands[key];
        if (command) {
            command.exec(world);
        } else {
            console.log(`Unknown command. Key: ${key}`);
        }
        world.updateInfo()
    }
                
    loadNextOctaMode(world: World) {
        const modes = ['brakke_1', 'brakke_2', 'brakke_3', 'brakke_4', 'brakke_5', 'brakke_a', 'brakke_b', 'brakke_c', 'brakke_d']
        this.octa_mode = (this.octa_mode+1) % modes.length
        const mode = modes[this.octa_mode]
        console.log(`Loading Octa(${mode})`)
        world.load(new Octa(mode))
    }

    htmlHelpString(): string {
        return `command keys:<ul>`+Object.entries(this.commands).map(([key, val]) => {
            return `<li><b>${key}</b>: ${val.description}</li>`
        }).join('')+'</ul>'
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
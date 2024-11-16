import World from './world'
import Commands from './commands'


const canvas = document.getElementById('my-canvas') as HTMLCanvasElement
const info_div = document.getElementById('my-info') || undefined
const commands_div = document.getElementById('my-commands')

const world = new World({canvas, info_div})
const commands = new Commands()

if (commands_div) {
	commands_div.innerHTML = commands.htmlHelpString()
}

commands.exec('0', world) // load pringle
document.addEventListener("keydown", evt => commands.exec(evt.key, world), false)



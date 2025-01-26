import World from './world'
import Commands from './commands'


const $canvas = document.getElementById('my-canvas') as HTMLCanvasElement
const $info = document.getElementById('my-info') || undefined
const $commands = document.getElementById('my-commands')
const $title = document.getElementById('my-title') || undefined

const world = new World({$canvas, $info, $title})
const commands = new Commands()

if ($commands) {
	$commands.innerHTML = commands.htmlHelpString()
}

commands.exec('0', world) // load pringle
document.addEventListener("keydown", evt => commands.exec(evt.key, world), false)



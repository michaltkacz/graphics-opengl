// --- PROGRAM ENTRY ---
// import Stats from '../node_modules/stats.js/src/Stats.js';
import Stats from 'https://cdnjs.cloudflare.com/ajax/libs/stats.js/r17/Stats.js';
import { SceneManager } from './sceneManager.js';

// Get canvas html element
const container = document.getElementById("container");
const canvas = document.getElementById("canvas");
// Create scene manager
const sceneManager = new SceneManager(canvas);
// Create stats
const stats = new Stats();
container.appendChild(stats.dom);


// All event bindings in this function
bindEventListeners();

// Update and render scene
updateAndRenderScene();


// --- Functions ---
function updateAndRenderScene() {
    requestAnimationFrame(updateAndRenderScene);
    stats.update();
    sceneManager.update();
    sceneManager.render();
}

function bindEventListeners() {
    window.onresize = resizeCanvas;
    resizeCanvas();
}

function resizeCanvas() {
    canvas.style.width = '100%';
    canvas.style.height = '100%';

    canvas.width = canvas.offsetWidth;
    canvas.height = canvas.offsetHeight;

    sceneManager.onWindowResize();
}
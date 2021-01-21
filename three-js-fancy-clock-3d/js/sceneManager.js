// import * as THREE from '../node_modules/three/build/three.module.js';
// import { OrbitControls } from '../node_modules/three/examples/jsm/controls/OrbitControls.js';
// import { EffectComposer } from '../node_modules/three/examples/jsm/postprocessing/EffectComposer.js';
// import { RenderPass } from '../node_modules/three/examples/jsm/postprocessing/RenderPass.js';
// import { UnrealBloomPass } from '../node_modules/three/examples/jsm/postprocessing/UnrealBloomPass.js';

import * as THREE from 'https://threejs.org/build/three.module.js';
import { OrbitControls } from 'https://threejs.org/examples/jsm/controls/OrbitControls.js';
import { EffectComposer } from 'https://threejs.org/examples/jsm/postprocessing/EffectComposer.js';
import { RenderPass } from 'https://threejs.org/examples/jsm/postprocessing/RenderPass.js';
import { UnrealBloomPass } from 'https://threejs.org/examples/jsm/postprocessing/UnrealBloomPass.js';

import { Axes } from './objects/axes.js';
import { Plane } from './objects/plane.js';
import { Clock } from './objects/clock/clock.js';
import { AmbientLight } from './objects/lights/ambientLight.js';

export class SceneManager {
    constructor(canvas) {
        this._canvas = canvas;
        this._scene = this._buildScene();
        this._camera = this._buildCamera(canvas.width, canvas.height);
        this._renderer = this._buildRenderer(canvas.width, canvas.height);
        this._sceneObjects = this._buildSceneObjects();
        this._cameraControls = this._buildCameraControls();
        this._renderPass = this._buildRenderPass();
        this._bloomPass = this._buildBloomPass();
        this._effectComposer = this._buildEffectComposer();
    }

    // --- Pseudo private methods ---
    _buildScene() {
        const scene = new THREE.Scene();
        scene.fog = new THREE.Fog(0x000000, 150, 300);
        return scene;
    }

    _buildCamera(width, height) {
        const fov = 75;
        const aspectRatio = width / height;
        const zNear = 1;
        const zFar = 1000;
        const camera = new THREE.PerspectiveCamera(fov, aspectRatio, zNear, zFar);
        camera.position.set(0, 0, -50);
        camera.lookAt(0, 0, 0);
        return camera;
    }

    _buildRenderer(width, height) {
        const renderer = new THREE.WebGLRenderer({ canvas: this._canvas, antialias: true });
        renderer.setClearColor(0x000000);
        renderer.setSize(width, height);
        renderer.toneMapping = THREE.ReinhardToneMapping;
        return renderer;
    }

    _buildSceneObjects() {
        const sceneObjects = {
            //"light": new AmbientLight(this._scene, 0x991f9c),
            //"axes": new Axes(this._scene),
            "plane": new Plane(this._scene, 1000, 1000, 0x06001a, -70, 32),
            "clock": new Clock(this._scene)
        };

        return sceneObjects;
    }

    _buildCameraControls() {
        const cameraControls = new OrbitControls(this._camera, this._renderer.domElement);
        cameraControls.autoRotate = false;
        cameraControls.autoRotateSpeed = 5.0;
        cameraControls.rotateSpeed = 0.5;
        cameraControls.enablePan = false;
        cameraControls.enableDamping = true;
        cameraControls.maxDistance = 100;
        cameraControls.minDistance = 10;
        return cameraControls;
    }

    _buildRenderPass() {
        return new RenderPass(this._scene, this._camera);
    }

    _buildBloomPass() {
        const bloomPass = new UnrealBloomPass(new THREE.Vector2(this._canvas.width, this._canvas.height), 0, 0, 0);
        bloomPass.exposure = 0;
        bloomPass.threshold = 0;
        bloomPass.strength = 0.9;
        bloomPass.radius = 0.2;
        return bloomPass;
    }

    _buildEffectComposer() {
        const effectComposer = new EffectComposer(this._renderer);
        effectComposer.addPass(this._renderPass);
        effectComposer.addPass(this._bloomPass);
        return effectComposer;
    }

    // --- Pseudo public core methods ---
    update() {
        this._cameraControls.update();
        this._sceneObjects["clock"].update();
        this._renderer.renderLists.dispose();
    }

    render() {
        //this._renderer.render(this._scene, this._camera);
        this._effectComposer.render();
    }

    // --- Pseudo public additional methods ---
    onWindowResize() {
        const { width, height } = canvas;

        this._camera.aspect = width / height;
        this._camera.updateProjectionMatrix();

        this._renderer.setSize(width, height);
        this._effectComposer.setSize(width, height);
    }
}
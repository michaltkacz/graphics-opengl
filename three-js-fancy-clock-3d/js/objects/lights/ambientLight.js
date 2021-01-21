// import * as THREE from '../../../node_modules/three/build/three.module.js';
import * as THREE from 'https://threejs.org/build/three.module.js';

export class AmbientLight {
    constructor(scene, color) {
        this._light = new THREE.AmbientLight(color);
        scene.add(this._light);
    }
}
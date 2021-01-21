// import * as THREE from '../../node_modules/three/build/three.module.js';
import * as THREE from 'https://threejs.org/build/three.module.js';

export class Plane {
    constructor(scene, width, height, color, yPosition, widthSegments = 1, heightSegments = 1) {
        const geometry = new THREE.PlaneGeometry(width, height, widthSegments, heightSegments);
        const material = new THREE.MeshPhongMaterial({ color: color, emissive: color, emissiveIntensity: 0.5 });
        const plane = new THREE.Mesh(geometry, material);
        plane.rotation.x = -Math.PI / 2;
        plane.position.y = yPosition;
        scene.add(plane);
    }

}
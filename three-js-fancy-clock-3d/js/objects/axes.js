// import * as THREE from '../../node_modules/three/build/three.module.js';
import * as THREE from 'https://threejs.org/build/three.module.js';

export class Axes {
    constructor(scene) {
        this._axisLength = 20;
        this._xAxis = this._buildAxis(0xff0000, new THREE.Vector3(1, 0, 0));
        this._yAxis = this._buildAxis(0x00ff00, new THREE.Vector3(0, 1, 0));
        this._zAxis = this._buildAxis(0x0000ff, new THREE.Vector3(0, 0, 1));
        scene.add(this._xAxis);
        scene.add(this._yAxis);
        scene.add(this._zAxis);
    }

    _buildAxis(axisColor, axisVectorMask) {
        const material = new THREE.LineBasicMaterial({ color: axisColor });

        const points = [];
        const lineEndPosition = new THREE.Vector3(this._axisLength, this._axisLength, this._axisLength).multiply(axisVectorMask);
        points.push(new THREE.Vector3(lineEndPosition.x, lineEndPosition.y, lineEndPosition.z));
        lineEndPosition.multiplyScalar(-1);
        points.push(new THREE.Vector3(lineEndPosition.x, lineEndPosition.y, lineEndPosition.z));

        const geometry = new THREE.BufferGeometry().setFromPoints(points);

        return new THREE.Line(geometry, material);
    }
}
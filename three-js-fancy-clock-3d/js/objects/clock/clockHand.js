// import * as THREE from '../../../node_modules/three/build/three.module.js';
import * as THREE from 'https://threejs.org/build/three.module.js';

export class ClockHand extends THREE.Object3D {
    static materialsSphereActive = {
        'bright': new THREE.MeshPhongMaterial({
            color: 0xF148FB,
            emissive: 0xF148FB,
            emissiveIntensity: 5
        }),
        'medium': new THREE.MeshPhongMaterial({
            color: 0x7122FA,
            emissive: 0x7122FA,
            emissiveIntensity: 5
        }),
        'dark': new THREE.MeshPhongMaterial({
            color: 0x560A86,
            emissive: 0x560A86,
            emissiveIntensity: 15
        })
    }

    static materialsSphereInactive = {
        'bright': new THREE.MeshPhongMaterial({
            color: 0xF148FB,
            emissive: 0xF148FB,
            emissiveIntensity: 0.1
        }),
        'medium': new THREE.MeshPhongMaterial({
            color: 0x7122FA,
            emissive: 0x7122FA,
            emissiveIntensity: 0.1
        }),
        'dark': new THREE.MeshPhongMaterial({
            color: 0x560A86,
            emissive: 0x560A86,
            emissiveIntensity: 0.1
        })
    }

    static geometrySphere = new THREE.SphereGeometry(1, 32, 32);

    constructor(clockMechanism, radius, numOfSpheres, numOfActiveSpheres, color) {
        super();
        this.rotation.set(0, 0, 0);
        this._spheres = []
        this._r = radius;
        this._color = color;
        this._lastTime = numOfActiveSpheres;

        for (let i = 0; i < numOfSpheres; i++) {
            if (i <= numOfActiveSpheres) {
                this._spheres[i] = this._buildSphere(ClockHand.materialsSphereActive[this._color], i, numOfSpheres);
            } else {
                this._spheres[i] = this._buildSphere(ClockHand.materialsSphereInactive[this._color], i, numOfSpheres);
            }
            this.add(this._spheres[i]);
        }
        clockMechanism.add(this);
    }

    _buildSphere(material, positionId, numOfSpheres) {
        const sphere = new THREE.Mesh(ClockHand.geometrySphere, material);
        const spherePosition = this._calculateSpherePosition(positionId, numOfSpheres);
        sphere.position.set(spherePosition.x, spherePosition.y, spherePosition.z);
        return sphere;
    }

    _calculateSpherePosition(positionId, numOfSpheres) {
        const angle = 2 * Math.PI * positionId / numOfSpheres;
        const x = this._r * Math.cos(angle);
        const y = this._r * Math.sin(angle);
        const z = 0;
        return new THREE.Vector3(x, y, z);
    }

    _activateSpheres(count) {
        for (let i = 0; i < this._spheres.length; i++) {
            this._spheres[i].material.dispose();
            if (i <= count) {
                this._spheres[i].material = ClockHand.materialsSphereActive[this._color];
            } else {
                this._spheres[i].material = ClockHand.materialsSphereInactive[this._color];
            }
        }
    }

    update(time, rotationAngles) {
        if (time !== this._lastTime) {
            this._activateSpheres(time);
            this._lastTime = time;
        }

        this.rotation.x += rotationAngles.x;
        this.rotation.y += rotationAngles.y;
        this.rotation.z += rotationAngles.z;
    }
}
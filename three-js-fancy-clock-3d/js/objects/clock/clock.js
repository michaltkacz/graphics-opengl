// import * as THREE from '../../../node_modules/three/build/three.module.js';
import * as THREE from 'https://threejs.org/build/three.module.js';

import { ClockMechanism } from './clockMechanism.js';
import { ClockDisplay } from './clockDisplay.js';

export class Clock extends THREE.Object3D {
    constructor(scene) {
        super();
        const date = new Date();
        this._clockDisplay = new ClockDisplay(this, date);
        this._clockMechanism = new ClockMechanism(this, date);
        scene.add(this);
    }

    update() {
        const newDate = new Date();
        this._clockDisplay.update(newDate);
        this._clockMechanism.update(newDate);
    }


}
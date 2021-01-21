// import * as THREE from '../../../node_modules/three/build/three.module.js';
import * as THREE from 'https://threejs.org/build/three.module.js';
import { ClockHand } from './clockHand.js';

export class ClockMechanism extends THREE.Object3D {
    constructor(clock, date) {
        super();
        this._hoursClockHand = new ClockHand(this, 15, 24, date.getHours(), 'dark');
        this._minutesClockHand = new ClockHand(this, 20, 60, date.getMinutes(), 'medium');
        this._secondsClockHand = new ClockHand(this, 25, 60, date.getSeconds(), 'bright');
        this.rotation.z = Math.PI / 2;
        clock.add(this);
    }

    update(date) {
        this._hoursClockHand.update(date.getHours(), new THREE.Vector3(0.001, 0, 0));
        this._minutesClockHand.update(date.getMinutes(), new THREE.Vector3(0, 0.001, 0));
        this._secondsClockHand.update(date.getSeconds(), new THREE.Vector3(0, 0, 0.001));
    }
}
// import * as THREE from '../../../node_modules/three/build/three.module.js';
import * as THREE from 'https://threejs.org/build/three.module.js';

export class ClockDisplay extends THREE.Object3D {
    static font = undefined;
    static loadFont() {
        const loader = new THREE.FontLoader();
        loader.load('https://threejs.org/examples/fonts/optimer_bold.typeface.json', function(response) {
            ClockDisplay.font = response;
        });
    }

    static geometryCircle = new THREE.CircleGeometry(4.5, 32);
    static materialCircle = new THREE.MeshPhongMaterial({
        color: 0x82E0BF,
        emissive: 0x82E0BF,
        emissiveIntensity: 0.5,
        side: THREE.DoubleSide
    });

    static geometryTorus = new THREE.TorusGeometry(4.5, 0.5, 16, 100);
    static materialTorus = new THREE.MeshPhongMaterial({
        color: 0x82E0BF,
        emissive: 0x82E0BF,
        emissiveIntensity: 2
    });

    static materialText = new THREE.MeshPhongMaterial({
        color: 0x120052,
        emissive: 0x120052,
        emissiveIntensity: 1
    });

    constructor(clock, date) {
        super();
        ClockDisplay.loadFont();
        this._lastDate = date;
        this.meshText1 = null;
        this.meshText2 = null;

        const circle = new THREE.Mesh(ClockDisplay.geometryCircle, ClockDisplay.materialCircle);
        this.add(circle);

        const torus = new THREE.Mesh(ClockDisplay.geometryTorus, ClockDisplay.materialTorus);
        this.add(torus);

        clock.add(this);
    }

    _removeClockDisplayText() {
        if (this.meshText1 !== null && this.meshText2 !== null) {
            this.meshText1.geometry.dispose();
            this.meshText2.geometry.dispose();
            this.meshText1.material.dispose();
            this.meshText2.material.dispose();
            this.remove(this.meshText1);
            this.remove(this.meshText2);
        }
    }

    _createClockDisplayText() {
        const timeString = this._getTimeString(this._lastDate);
        const geometryText = new THREE.TextGeometry(timeString, {
            font: ClockDisplay.font,
            size: 1.2,
            height: 0.2,
            curveSegments: 12,
            bevelEnabled: false,
            bevelThickness: 10,
            bevelSize: 8,
            bevelOffset: 0,
            bevelSegments: 5
        });

        geometryText.computeBoundingBox();
        const centerOffsetX = -0.5 * (geometryText.boundingBox.max.x - geometryText.boundingBox.min.x);
        const centerOffsetY = -0.5 * (geometryText.boundingBox.max.y - geometryText.boundingBox.min.y);

        this.meshText1 = new THREE.Mesh(geometryText, ClockDisplay.materialText);
        this.meshText2 = this.meshText1.clone();

        this.meshText1.position.x = centerOffsetX;
        this.meshText1.position.y = centerOffsetY;
        this.meshText1.position.z = 0.01;

        this.meshText2.rotation.y = Math.PI;
        this.meshText2.position.x = -centerOffsetX;
        this.meshText2.position.y = centerOffsetY;
        this.meshText2.position.z = -0.01;

        this.add(this.meshText1);
        this.add(this.meshText2);
    }

    _getTimeString(date) {
        const h = date.getHours();
        const hours = h < 10 ? "0" + h : h;

        const m = date.getMinutes();
        const minutes = m < 10 ? "0" + m : m;

        const s = date.getSeconds();
        const seconds = s < 10 ? "0" + s : s;
        return hours + ":" + minutes + ":" + seconds;
    }

    update(date) {
        if (ClockDisplay.font === undefined) {
            return;
        }

        if (date.getSeconds() === this._lastDate.getSeconds()) {
            return;
        }

        this._lastDate = date;
        this._removeClockDisplayText();
        this._createClockDisplayText();
    }
}
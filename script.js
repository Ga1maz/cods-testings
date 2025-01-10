const map = L.map('map', {
    zoomControl: true
}).setView([55.751244, 37.618423], 10);

L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
}).addTo(map);

L.control.zoom({
    position: 'bottomright'
}).addTo(map);

let marker1 = L.marker([55.751244, 37.618423]).addTo(map);
let marker2 = L.marker([55.760244, 37.628423]).addTo(map);

let polyline = L.polyline([marker1.getLatLng(), marker2.getLatLng()], { color: 'blue' }).addTo(map);

const client = mqtt.connect('wss://mqtt.cloa.su:8080', {
    username: 'ga1maz',
    password: 'almazg1234'
});

client.on("connect", () => {
    console.log("Подключено к MQTT серверу");
    client.subscribe("tfmini/distance");
    client.subscribe("tfmini/strength");
    client.subscribe("dht22/temperature");
    client.subscribe("dht22/humidity");
    client.subscribe("ina219/volt");
    client.subscribe("ina219/current");
    client.subscribe("ina219/loadvoltage"); 
    client.subscribe("ina219/power");
    client.subscribe("battery/charge");
    client.subscribe("gps/coordinates");
    client.subscribe("gps/cord2");
});

client.on('message', (topic, message) => {
    const data = message.toString();
    switch (topic) {
        case 'gps/coordinates':
            updateMarker1(data);
            break;
        case 'gps/cord2':
            updateMarker2(data);
            break;
        case 'tfmini/distance':
            updateDistanceData(data);
            break;
        case 'tfmini/strength':
            updateStrengthData(data);
            break;
        case 'dht22/temperature':
            updateTemperatureData(data);
            break;
        case 'dht22/humidity':
            updateHumidityData(data);
            break;
        case 'ina219/volt':
            updateVoltageData(data);
            break;
        case 'ina219/current':
            updateCurrentData(data);
            break;
        case 'ina219/loadvoltage':
            updateLoadVoltageData(data);
            break;
        case 'ina219/power':
            updatePowerData(data);
            break;
        case 'battery/charge':
            updateBatteryData(data);
            break;
        default:
            console.log(`Неизвестный топик: ${topic}`);
    }
});

client.on('error', (error) => {
    console.error('Ошибка подключения:', error);
});

function updateMarker1(data) {
    console.log(`Получено сообщение для первой точки: ${data}`);
    const coordinates = data.split(',');
    const lat1 = parseFloat(coordinates[0]);
    const lon1 = parseFloat(coordinates[1]);

    if (!isNaN(lat1) && !isNaN(lon1)) {
        marker1.setLatLng([lat1, lon1]);
        marker1.bindPopup(`<b>Точка 1:</b><br>${lat1.toFixed(6)}, ${lon1.toFixed(6)}`).openPopup();
        polyline.setLatLngs([marker1.getLatLng(), marker2.getLatLng()]);

        updateDistance();

        map.flyToBounds([marker1.getLatLng(), marker2.getLatLng()], {
            padding: [50, 50],
            animate: true
        });
    } else {
        console.error('Некорректные координаты для первой точки:', coordinates);
    }
}

function updateMarker2(data) {
    console.log(`Получено сообщение для второй точки: ${data}`);
    const coordinates = data.split(',');
    const lat2 = parseFloat(coordinates[0]);
    const lon2 = parseFloat(coordinates[1]);

    if (!isNaN(lat2) && !isNaN(lon2)) {
        marker2.setLatLng([lat2, lon2]);
        marker2.bindPopup(`<b>Точка 2:</b><br>${lat2.toFixed(6)}, ${lon2.toFixed(6)}`).openPopup();
        polyline.setLatLngs([marker1.getLatLng(), marker2.getLatLng()]);

        updateDistance();

        map.flyToBounds([marker1.getLatLng(), marker2.getLatLng()], {
            padding: [50, 50],
            animate: true
        });
    } else {
        console.error('Некорректные координаты для второй точки:', coordinates);
    }
}

function updateDistance() {
    const latlng1 = marker1.getLatLng();
    const latlng2 = marker2.getLatLng();
    const distance = latlng1.distanceTo(latlng2); // Расстояние в метрах
    console.log(`Расстояние между маркерами: ${distance.toFixed(2)} метров`);

    document.getElementById("point1").innerText = `Точка 1: ${latlng1.lat.toFixed(6)}, ${latlng1.lng.toFixed(6)}`;
    document.getElementById("point2").innerText = `Точка 2: ${latlng2.lat.toFixed(6)}, ${latlng2.lng.toFixed(6)}`;
    document.getElementById("distance-between").innerText = `Расстояние между точками: ${distance.toFixed(2)} метров`;
}

function updateDistanceData(data) {
    document.getElementById("distance").innerText = `Дистанция: ${data} мм`;
}

function updateStrengthData(data) {
    document.getElementById("strength").innerText = `Сила сигнала: ${data}`;
}

function updateTemperatureData(data) {
    const temperature = parseFloat(data);
    document.getElementById("temperature").innerText = `Температура (°C): ${temperature}`;
    updateTemperatureChart(temperature);

    const thermostatStatus = document.getElementById("thermostat-status");
    if (temperature > 15) {
        thermostatStatus.innerText = 'Термостат: Не работает';
        thermostatStatus.style.backgroundColor = 'rgb(255, 0, 0)';
    } else {
        thermostatStatus.innerText = 'Термостат: Работает';
        thermostatStatus.style.backgroundColor = 'rgb(9, 255, 0)';
    }
}

function updateHumidityData(data) {
    document.getElementById("humidity").innerText = `Влажность (%): ${data}`;
}

function updateVoltageData(data) {
    document.getElementById("voltage").innerText = `Напряжение (V): ${data}`;
}

function updateCurrentData(data) {
    document.getElementById("current").innerText = `Ток (mA): ${data}`;
}

function updateLoadVoltageData(data) {
    document.getElementById("loadvoltage").innerText = `Нагрузка (V): ${data}`;
}

function updatePowerData(data) {
    document.getElementById("power").innerText = `Мощность (mW): ${data}`;
}

function updateBatteryData(data) {
    const charge = parseInt(data);
    document.getElementById("battery").innerText = `Заряд: ${charge}%`;
    updateBatteryChart(charge);
    updateBatteryBox(charge);

    const batteryReplacement = document.getElementById("battery-replacement");
    if (charge < 15) {
        batteryReplacement.innerText = 'Замена аккумулятора: Требуется';
        batteryReplacement.style.backgroundColor = 'rgb(255, 0, 0)';
    } else {
        batteryReplacement.innerText = 'Замена аккумулятора: Не требуется';
        batteryReplacement.style.backgroundColor = 'rgb(9, 255, 0)';
    }
}

function updateBatteryBox(charge) {
    const batteryBox = document.getElementById("battery");
    batteryBox.innerText = `Заряд: ${charge}%`;

    if (charge > 66) {
        batteryBox.style.backgroundColor = 'rgb(9, 255, 0)';
    } else if (charge > 33) {
        batteryBox.style.backgroundColor = 'rgb(187, 255, 0)';
    } else {
        batteryBox.style.backgroundColor = 'rgb(255, 0, 0)';
    }
}

function initializeCharts() {
    const batteryCtx = document.getElementById('battery-chart').getContext('2d');
    batteryChart = new Chart(batteryCtx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: 'Заряд АКБ (%)',
                data: [],
                borderColor: 'rgb(255, 0, 0)',
                tension: 0.1
            }]
        },
        options: {
            responsive: true,
            scales: {
                x: {
                    title: {
                        display: true,
                        text: 'Время'
                    }
                },
                y: {
                    title: {
                        display: true,
                        text: 'Заряд (%)'
                    },
                    min: 0,
                    max: 100
                }
            }
        }
    });

    const temperatureCtx = document.getElementById('temperature-chart').getContext('2d');
    temperatureChart = new Chart(temperatureCtx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: 'Температура (°C)',
                data: [],
                borderColor: 'rgb(9, 255, 0)',
                tension: 0.1
            }]
        },
        options: {
            responsive: true,
            scales: {
                x: {
                    title: {
                        display: true,
                        text: 'Время'
                    }
                },
                y: {
                    title: {
                        display: true,
                        text: 'Температура (°C)'
                    }
                }
            }
        }
    });
}

function updateBatteryChart(value) {
    const now = new Date().toLocaleTimeString();
    if (batteryChart.data.labels.length > 20) {
        batteryChart.data.labels.shift();
        batteryChart.data.datasets[0].data.shift();
    }
    batteryChart.data.labels.push(now);
    batteryChart.data.datasets[0].data.push(value);
    batteryChart.update();
}

function updateTemperatureChart(value) {
    const now = new Date().toLocaleTimeString();
    if (temperatureChart.data.labels.length > 20) {
        temperatureChart.data.labels.shift();
        temperatureChart.data.datasets[0].data.shift();
    }
    temperatureChart.data.labels.push(now);
    temperatureChart.data.datasets[0].data.push(value);
    temperatureChart.update();
}

initializeCharts();

function updateLoadingText(text) {
    document.getElementById('loadingText').innerText = text;
}

function simulateLoading() {
    return new Promise((resolve) => {
        const loadingSteps = [
            "Подгрузка CSS Leaflet",
            "Подгрузка JS Leaflet",
            "Подгрузка JS Mqtt",
            "Подгрузка CSS самого сайта",
            "Подгрузка JS самого сайта",
            "Соединено с MQTT"
        ];

        let step = 0;

        const interval = setInterval(() => {
            if (step < loadingSteps.length) {
                updateLoadingText(loadingSteps[step]);
                step++;
            } else {
                clearInterval(interval);
                resolve();
            }
        }, 500);
    });
}

window.onload = async function () {
    await simulateLoading();

    setTimeout(function () {
        const loadingOverlay = document.getElementById('loadingOverlay');
        loadingOverlay.style.transition = 'opacity 1s';
        loadingOverlay.style.opacity = '0';

        setTimeout(function () {
            loadingOverlay.style.display = 'none';
        }, 1000);
    }, 3000); 
};

document.querySelectorAll('.container').forEach(el => el.classList.add('shadow'));
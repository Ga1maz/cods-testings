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

let infoPanel = L.control({ position: 'topright' });
infoPanel.onAdd = function () {
    this._div = L.DomUtil.create('div', 'info');
    this.update();
    return this._div;
};

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
<<<<<<< HEAD
    client.subscribe("ina219/loadvoltage"); 
=======
    client.subscribe("ina219/loadvoltage");
>>>>>>> 26e8e67 (UpDate)
    client.subscribe("ina219/power");
    client.subscribe("battery/charge");
    client.subscribe("gps/coordinates");
    client.subscribe("gps/cord2");
});

client.on('message', (topic, message) => {
    if (topic === 'gps/coordinates') {
        console.log(`Получено сообщение для первой точки: ${message.toString()}`);
        const coordinates = message.toString().split(',');
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

    if (topic === 'gps/cord2') {
        console.log(`Получено сообщение для второй точки: ${message.toString()}`);
        const coordinates = message.toString().split(',');
        const lat2 = parseFloat(coordinates[0]);
        const lon2 = parseFloat(coordinates[1]);

        if (!isNaN(lat2) && !isNaN(lon2)) {
            marker2.setLatLng([lat2, lon2]);
            marker2.bindPopup(`<b>Точка 2:</b><br>${lat2.toFixed(6)}, ${lon2.toFixed(6)}`).openPopup();
            polyline.setLatLngs([marker1.getLatLng(), marker2.getLatLng()]);

            updateDistance();

            // Автозум, чтобы показать обе точки на карте
            map.flyToBounds([marker1.getLatLng(), marker2.getLatLng()], {
                padding: [50, 50],
                animate: true
            });
        } else {
            console.error('Некорректные координаты для второй точки:', coordinates);
        }
    }
});

client.on('error', (error) => {
    console.error('Ошибка подключения:', error);
});

function updateDistance() {
    const latlng1 = marker1.getLatLng();
    const latlng2 = marker2.getLatLng();
    const distance = latlng1.distanceTo(latlng2); // Расстояние в метрах
    console.log(`Расстояние между маркерами: ${distance.toFixed(2)} метров`);

    infoPanel.update(
        `${latlng1.lat.toFixed(6)}, ${latlng1.lng.toFixed(6)}`,
        `${latlng2.lat.toFixed(6)}, ${latlng2.lng.toFixed(6)}`,
        distance.toFixed(2)
    );
}
let batteryChart;
let temperatureChart;

client.on("message", (topic, message) => {
    const data = message.toString();
    switch (topic) {
        case "tfmini/distance":
            document.getElementById("distance").innerText = `Дистанция: ${data} мм`;
            break;
        case "tfmini/strength":
            document.getElementById("strength").innerText = `Сила сигнала: ${data}`;
            break;
        case "dht22/temperature":
            document.getElementById("temperature").innerText = `Температура (°C): ${data}`;
            updateTemperatureChart(parseFloat(data));
            break;
        case "dht22/humidity":
            document.getElementById("humidity").innerText = `Влажность (%): ${data}`;
            break;
        case "ina219/volt":
            document.getElementById("voltage").innerText = `Напряжение (V): ${data}`;
            break;
        case "ina219/current":
            document.getElementById("current").innerText = `Ток (mA): ${data}`;
            break;
        case "ina219/loadvoltage":
            document.getElementById("loadvoltage").innerText = `Нагрузка (V): ${data}`;
            break;
        case "ina219/power":
            document.getElementById("power").innerText = `Мощность (mW): ${data}`;
            break;
        case "battery/charge":
            const charge = parseInt(data);
            document.getElementById("battery").innerText = `Заряд: ${charge}%`;
            updateBatteryChart(charge);
            updateBatteryBox(charge);
            break;
        default:
            console.log(`Неизвестный топик: ${topic}`);
    }
});

client.on("error", (err) => {
    console.error("Ошибка подключения:", err);
});

function updateBatteryBox(charge) {
    const batteryBox = document.getElementById("battery");
    batteryBox.innerText = `Заряд: ${charge}%`;

    if (charge > 66) {
        batteryBox.style.backgroundColor = 'green';
    } else if (charge > 33) {
        batteryBox.style.backgroundColor = 'yellow';
    } else {
        batteryBox.style.backgroundColor = 'red';
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
                borderColor: 'rgb(75, 192, 192)',
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
                borderColor: 'rgb(255, 99, 132)',
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

window.onload = function () {
    setTimeout(function () {
        const loadingOverlay = document.getElementById('loadingOverlay');
        loadingOverlay.style.transition = 'opacity 1s';
        loadingOverlay.style.opacity = '0';

        setTimeout(function () {
            loadingOverlay.style.display = 'none';
        }, 2000);
    }, 5000);
    if (window.innerWidth < 768) {
        document.getElementById('mobileWarning').style.display = 'flex';
    }
<<<<<<< HEAD
};
=======
};
>>>>>>> 26e8e67 (UpDate)

ymaps.ready(init);

function init() {
    console.log("Инициализация карты...");
    const map = new ymaps.Map('map', {
        center: [55.751244, 37.618423], // Центр карты
        zoom: 10, // Уровень масштабирования
        controls: ['zoomControl'], // Контролы
        type: 'yandex#map' // Тип карты
    }, {
        restrictMapArea: [
            [55.5, 37.0],
            [56.0, 38.0]
        ]
    });

    // Устанавливаем тему "Серая ночь"
    const layer = new ymaps.Layer('https://core-renderer-tiles.maps.yandex.net/tiles?l=map&v=19.02.2021&x=%x&y=%y&z=%z&scale=1&lang=ru_RU', {
        projection: ymaps.projection.sphericalMercator
    });
    map.layers.add(layer);

    // Создание маркеров
    const marker1 = new ymaps.Placemark([55.751244, 37.618423], {
        balloonContent: '<b>Точка 1:</b><br>55.751244, 37.618423'
    });
    const marker2 = new ymaps.Placemark([55.760244, 37.628423], {
        balloonContent: '<b>Точка 2:</b><br>55.760244, 37.628423'
    });

    // Добавление маркеров на карту
    map.geoObjects.add(marker1);
    map.geoObjects.add(marker2);

    // Создание линии между маркерами
    const polyline = new ymaps.Polyline([
        [55.751244, 37.618423],
        [55.760244, 37.628423]
    ], {}, {
        strokeColor: '#0000FF',
        strokeWidth: 4
    });

    // Добавление линии на карту
    map.geoObjects.add(polyline);

    // Подключение к MQTT серверу
    console.log("Подключение к MQTT серверу...");
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
        console.log(`Получено сообщение от ${topic}: ${data}`);
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
            marker1.geometry.setCoordinates([lat1, lon1]);
            marker1.properties.set('balloonContent', `<b>Точка 1:</b><br>${lat1.toFixed(6)}, ${lon1.toFixed(6)}`);
            polyline.geometry.setCoordinates([[lat1, lon1], marker2.geometry.getCoordinates()]);
            updateDistance();
            map.setBounds(map.geoObjects.getBounds(), { checkZoomRange: true, zoomMargin: 50 });
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
            marker2.geometry.setCoordinates([lat2, lon2]);
            marker2.properties.set('balloonContent', `<b>Точка 2:</b><br>${lat2.toFixed(6)}, ${lon2.toFixed(6)}`);
            polyline.geometry.setCoordinates([marker1.geometry.getCoordinates(), [lat2, lon2]]);
            updateDistance();
            map.setBounds(map.geoObjects.getBounds(), { checkZoomRange: true, zoomMargin: 50 });
        } else {
            console.error('Некорректные координаты для второй точки:', coordinates);
        }
    }

    function updateDistance() {
        const latlng1 = marker1.geometry.getCoordinates();
        const latlng2 = marker2.geometry.getCoordinates();
        const distance = ymaps.coordSystem.geo.getDistance(latlng1, latlng2); // Расстояние в метрах
        console.log(`Расстояние между маркерами: ${distance.toFixed(2)} метров`);

        document.getElementById("point1").innerText = `Точка 1: ${latlng1[0].toFixed(6)}, ${latlng1[1].toFixed(6)}`;
        document.getElementById("point2").innerText = `Точка 2: ${latlng2[0].toFixed(6)}, ${latlng2[1].toFixed(6)}`;
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
        console.log("Инициализация графиков...");
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

    document.querySelectorAll('.container').forEach(el => el.classList.add('shadow'));
}
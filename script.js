// Инициализация карты
const map = L.map('map').setView([55.751244, 37.618423], 10); // Москва

// Добавление слоя карты
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
}).addTo(map);

// Инициализация маркеров
let marker1 = L.marker([55.751244, 37.618423]).addTo(map); // Первое местоположение
let marker2 = L.marker([55.760244, 37.628423]).addTo(map); // Второе местоположение (пример)

// Линия между маркерами
let polyline = L.polyline([marker1.getLatLng(), marker2.getLatLng()], { color: 'blue' }).addTo(map);

// Создание элемента для отображения расстояния
let infoPanel = L.control({ position: 'topright' });
infoPanel.onAdd = function () {
    this._div = L.DomUtil.create('div', 'info'); // Создание контейнера
    this.update(); // Инициализация с пустыми данными
    return this._div;
};
infoPanel.update = function (coords1 = '', coords2 = '', distance = '') {
    this._div.innerHTML = `
        <h3 style="font-size: 18px; font-weight: bold; color: red;">ОТЛАДКА</h4>
        <p style="font-size: 16px; color: red;">Точка 1: <strong>${coords1}</strong></p>
        <p style="font-size: 16px; color: red;">Точка 2: <strong>${coords2}</strong></p>
        <p style="font-size: 16px; color: red;">Расстояние: <strong>${distance}</strong> м</p>
    `;
};
infoPanel.addTo(map);

// Подключение к MQTT через WebSocket
const client = mqtt.connect('wss://mqtt.cloa.su:8080', {
    username: 'ga1maz',
    password: 'almazg1234'
});

client.on('connect', () => {
    console.log('Подключено к MQTT');
    client.subscribe('gps/coordinates', (err) => {
        if (!err) {
            console.log('Подписались на тему: gps/coordinates');
        } else {
            console.error('Ошибка подписки:', err);
        }
    });
    
    client.subscribe('gps/cord2', (err) => {
        if (!err) {
            console.log('Подписались на тему: gps/cord2');
        } else {
            console.error('Ошибка подписки:', err);
        }
    });
});

client.on('message', (topic, message) => {
    if (topic === 'gps/coordinates') {
        console.log(`Получено сообщение для первой точки: ${message.toString()}`);
        const coordinates = message.toString().split(',');
        const lat1 = parseFloat(coordinates[0]);
        const lon1 = parseFloat(coordinates[1]);

        if (!isNaN(lat1) && !isNaN(lon1)) {
            marker1.setLatLng([lat1, lon1]);
            marker1.bindPopup(`<b>Точка 1:</b><br>${lat1.toFixed(6)}, ${lon1.toFixed(6)}`).openPopup(); // Добавляем всплывающее окно
            polyline.setLatLngs([marker1.getLatLng(), marker2.getLatLng()]); // Обновляем линию

            updateDistance(); // Обновляем расстояние

            // Плавно показываем обе точки на карте
            map.flyToBounds([marker1.getLatLng(), marker2.getLatLng()], {
                padding: [50, 50], // Немного отступов, чтобы сделать отображение комфортным
                animate: true // Включаем анимацию
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
            marker2.bindPopup(`<b>Точка 2:</b><br>${lat2.toFixed(6)}, ${lon2.toFixed(6)}`).openPopup(); // Добавляем всплывающее окно
            polyline.setLatLngs([marker1.getLatLng(), marker2.getLatLng()]); // Обновляем линию

            updateDistance(); // Обновляем расстояние

            // Плавно показываем обе точки на карте
            map.flyToBounds([marker1.getLatLng(), marker2.getLatLng()], {
                padding: [50, 50], // Немного отступов, чтобы сделать отображение комфортным
                animate: true // Включаем анимацию
            });
        } else {
            console.error('Некорректные координаты для второй точки:', coordinates);
        }
    }
});

client.on('error', (error) => {
    console.error('Ошибка подключения:', error);
});

// Функция для обновления расстояния между маркерами
function updateDistance() {
    const latlng1 = marker1.getLatLng();
    const latlng2 = marker2.getLatLng();
    const distance = latlng1.distanceTo(latlng2); // Расстояние в метрах
    console.log(`Расстояние между маркерами: ${distance.toFixed(2)} метров`);

    // Обновляем информацию в панели
    infoPanel.update(
        `${latlng1.lat.toFixed(6)}, ${latlng1.lng.toFixed(6)}`,
        `${latlng2.lat.toFixed(6)}, ${latlng2.lng.toFixed(6)}`,
        distance.toFixed(2)
    );
}

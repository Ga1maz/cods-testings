html,
body {
    height: 100%;
    margin: 0;
    padding: 0;
    overflow: hidden;
}

body {
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 0;
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    align-items: flex-start;
    height: 100vh;
    background: url('bak.jpg') no-repeat center center fixed;
    background-size: cover;
    overflow: hidden;
}

.container {
    background: rgba(255, 255, 255, 0.1);
    backdrop-filter: blur(10px);
    border-radius: 15px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
    padding: 20px;
    margin: 10px;
    overflow: hidden;
    height: calc(100vh - 93px);
    display: flex;
    flex-direction: column;
    opacity: 0;
    animation: slideUp 1s forwards;
}

.container>* {
    opacity: 0;
    animation: fadeInElement 1s forwards;
}

.container>*:nth-child(1) {
    animation-delay: 1s;
}

.container>*:nth-child(2) {
    animation-delay: 1.2s;
}

.container>*:nth-child(3) {
    animation-delay: 1.4s;
}

.container>*:nth-child(4) {
    animation-delay: 1.6s;
}

.map-container {
    flex: 1;
    max-width: 30%;
}

.status-container {
    flex: 1;
    max-width: 40%;
}

.graph-container {
    flex: 1;
    max-width: 30%;
    display: flex;
    flex-direction: column;
    gap: 20px;
}

#map {
    width: 100%;
    height: 300px;
    border-radius: 10px;
    box-shadow: 0 4px 15px rgba(0, 0, 0, 0.5);
}

.chart {
    flex: 1;
    background: rgba(255, 255, 255, 0.05);
    padding: 15px;
    border-radius: 15px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
}

.data-box {
    margin: 10px 0;
    padding: 15px;
    border-radius: 10px;
    background-color: rgba(255, 255, 255, 0.021);
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
    font-size: 18px;
    white-space: nowrap;
    opacity: 0;
    animation: fadeIn 1s forwards;
}

.battery-box {
    margin: 10px 0;
    padding: 15px;
    border-radius: 10px;
    text-align: center;
    font-size: 18px;
    font-weight: bold;
}

.leaflet-control-attribution {
    display: none;
}

.gps-info {
    margin-top: 15px;
}

.loading-overlay {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background-color: black;
    display: flex;
    justify-content: center;
    align-items: center;
    z-index: 9999;
}

.loading-container {
    display: flex;
    justify-content: center;
    align-items: center;
    padding: 10px;
}

.loading-container .dot {
    width: 40px;
    height: 40px;
    background-color: black;
    border-radius: 50%;
    animation: loading 1s infinite alternate;
}

.loading-container .dot:nth-child(1) {
    background-color: #4285f4;
    animation-delay: -0.25s;
}

.loading-container .dot:nth-child(2) {
    background-color: #ea4335;
    animation-delay: -0.5s;
}

.loading-container .dot:nth-child(3) {
    background-color: #fbbc05;
    animation-delay: -0.75s;
}

.loading-container .dot:nth-child(4) {
    background-color: #34a853;
    animation-delay: -1s;
}

@keyframes loading {
    0% {
        transform: translateY(-15px);
    }

    100% {
        transform: translateY(5px);
    }
}

@keyframes fadeIn {
    to {
        opacity: 1;
    }
}

@keyframes fadeInElement {
    from {
        opacity: 0;
        transform: translateY(20px);
    }

    to {
        opacity: 1;
        transform: translateY(0);
    }
}

@keyframes slideUp {
    from {
        opacity: 0;
        transform: translateY(100px);
    }

    to {
        opacity: 1;
        transform: translateY(0);
    }
}

.mobile-warning {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background-color: black;
    color: white;
    display: none;
    justify-content: center;
    align-items: center;
    flex-direction: column;
    z-index: 10000;
    text-align: center;
}

.mobile-warning .icon {
    color: red;
    font-size: 50px;
    margin-bottom: 20px;
}

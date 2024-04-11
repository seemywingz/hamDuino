

window.onload = function () {
}

var visualizationSelect = document.getElementById('visualizationSelect');
visualizationSelect.addEventListener('change', function () {
    var visualization = visualizationSelect.value;

    var wifiSettings = document.getElementById('wifi-settings');
    wifiSettings.style.display = 'none';

    var aboutSettings = document.getElementById('about-settings');
    aboutSettings.style.display = 'none';

    switch (visualization) {
        case 'wifi':
            wifiSettings.style.display = 'block';
            brightnessSettings.style.display = 'none';
            break;
        case 'about':
            aboutSettings.style.display = 'block';
            brightnessSettings.style.display = 'none';
            break;
    }
});

document.getElementById('wifiSetup').addEventListener('click', function () {
    window.location.href = '/setup';
});

document.getElementById('wifiErase').addEventListener('click', function () {
    window.location.href = '/erase';
});


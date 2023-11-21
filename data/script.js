function updateLEDstate() {
    fetch('/led_state')
        .then(response => response.text())
        .then(state => {
            document.getElementById("led_state").textContent = state;
        })
}
function turnOnLED() {
    fetch("/led_on");
    updateLEDstate();
}
function turnOffLED() {
    fetch("/led_off");
    updateLEDstate();
}
setInterval(() => {
    fetch('/temperature')
        .then(response => response.text())
        .then(temperature => {
            document.getElementById("temperature").textContent = temperature;
        });
}, 200);
setInterval(() => {
    fetch('/humidity')
        .then(response => response.text())
        .then(humidity => {
            document.getElementById("humidity").textContent = humidity;
        });
}, 200);

let options = document.querySelectorAll("select#rotate-degree option");
let ccw_checkbox = document.getElementById("CCW-checkbox");
function rotateStepper() {
    let n_options = options.length;
    for (let it = 0; it < n_options; it++) {
        if (options[it].selected) {
            if (ccw_checkbox.checked) {
                fetch("/rotate?degree=" + options[it].value + "&ccw=true");
            }
            else {
                fetch("/rotate?degree=" + options[it].value + "&ccw=false");
            }
            break;
        }
    }
}
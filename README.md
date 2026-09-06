# ⚡ ESP32 DASHBOARD & MOD CONTROLLER

Центр управления и прошивка для аппаратного контроллера импульсной нагрузки на базе микроконтроллера ESP32. Проект связывает прошивку чипа с десктопным графическим клиентом на C++ (Dear ImGui) для настройки параметров и мониторинга состояния устройства в реальном времени.

### ⚙️ Основные возможности

* <img src="https://api.iconify.design/lucide:sliders.svg" width="16" height="16" /> **Аппаратный ШИМ:** 8-битная регулировка мощности (0–255) на частоте 5 кГц.
* <img src="https://api.iconify.design/lucide:shield-check.svg" width="16" height="16" /> **Безопасность:** жесткая аппаратная отсечка времени подачи нагрузки (от 1 до 10 секунд).
* <img src="https://api.iconify.design/lucide:gauge.svg" width="16" height="16" /> **Телеметрия:** отображение расчетного напряжения, мощности и таймера затяжки на встроенном OLED-дисплее 0.96" (SSD1306) и в GUI.
* <img src="https://api.iconify.design/lucide:cable.svg" width="16" height="16" /> **Управление по UART:** изменение профилей мощности и отправка тестовых импульсов без перепрошивки микроконтроллера.
* <img src="https://api.iconify.design/lucide:binary.svg" width="16" height="16" /> **Автономная сборка:** автоматическая компиляция десктопного приложения под Windows через GitHub Actions.

### 🧰 Компоненты

* <img src="https://api.iconify.design/lucide:cpu.svg" width="16" height="16" /> **ESP32 DevKit** — основной вычислительный модуль.
* <img src="https://api.iconify.design/lucide:zap.svg" width="16" height="16" /> **N-канальный MOSFET (IRL3034 / LR7843)** — силовая коммутация нагрузки.
* <img src="https://api.iconify.design/lucide:tv.svg" width="16" height="16" /> **OLED SSD1306 (I2C)** — экран статуса и мощности.
* <img src="https://api.iconify.design/lucide:battery-charging.svg" width="16" height="16" /> **Аккумулятор 18650 / Li-Po** — питание силовой цепи.

### 📥 Установка и запуск

1. <img src="https://api.iconify.design/lucide:tag.svg" width="16" height="16" /> Перейдите во вкладку [Releases](../../releases) репозитория.
2. <img src="https://api.iconify.design/lucide:download.svg" width="16" height="16" /> Скачайте актуальную сборку прошивки (`firmware.bin`) и архив с панелью управления (`ESP32Dashboard.exe`).
3. <img src="https://api.iconify.design/lucide:flame.svg" width="16" height="16" /> Залейте бинарник в контроллер через Web Flasher или esptool.
4. <img src="https://api.iconify.design/lucide:play.svg" width="16" height="16" /> Подключите плату по USB и запустите `ESP32Dashboard.exe`.

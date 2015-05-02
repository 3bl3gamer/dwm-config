Доконфигурированный dwm-6.0.

С патчами [pertag](http://dwm.suckless.org/patches/pertag),
[systray](http://dwm.suckless.org/patches/systray)
и парой самописных функций для регулирования посветки.

`dwminit` - скрипт,
при запуске вырубает запущенные ранее копии себя (если таковые были),
файл настройки яркости разрешает для записи,
запускает `gnome-settings-daemon` и `nm-applet`,
конфигурирует раскладки через `setxkbmap`,
запускает цикл, обновляющий часики и заряд батареи
ну и запускает сам dwm.

Прописан в `/usr/share/xsessions/dwm.desktop`:
```
[Desktop Entry]
Encoding=UTF-8
Name=Dwm
Comment=Dynamic window manager
Exec=/home/zblzgamer/dwminit
Icon=dwm
Type=XSession
```

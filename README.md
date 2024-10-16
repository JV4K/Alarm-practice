# Alarm-practice
 
## Задача
Вкратце - сделать будильник на основе STM32 с использованием RTC и его alarm прерывания, с возможностью установки текущего времени и времени срабатывания будильника кнопками.

## Исходные данные
В налиичии имелся китайский отладочный комплект  [STM32 F4VE](https://stm32-base.org/boards/STM32F407VET6-STM32-F4VE-V2.0.html) с микроконтроллером STM32F407VET6 на борту. Также на плате имеется часовой кварц и батарейка для поддержания RTC, так что она отлично подходит под выполнение задачи.

<img src="https://stm32-base.org/assets/img/boards/STM32F407VET6_STM32_F4VE_V2.0-1.jpg" width="600">

## Ход работы
### State machine
Проект был реализован в соответствии с приведенной ниже машиной состояний.
![image](https://github.com/user-attachments/assets/655aa7fc-b9f2-459b-9b41-eaf75f65cd6a)
Состояния:
- IDLE: ничего не происходит; время идет (как и в любом другом режиме)
- RING: непосредственно срабатывание будильника, реализовано морганием светодиода
- SET_TIME: Установка времени кнопками
- SET_ALARM: Установка будильника кнопками
Из нее видно, что было решено использовать две кнопки, которые также имеются на отладочной плате.

## Комментарии по реализации
### Базовое прерывание
Все процессы вызываются базовым прерыванием по переполнению таймера, которое происходит с частотой 100 Гц.
### RTC
Сконфигурирован бортовой RTC с тактированием от часового кварца, а также активировано прерывание будильника. При старте микроконтроллера время не прописывается заново, так что благодаря батарейке на плате, оно сохраняется при отсутствии внешнего питания.
### Кнопки
Для чтения состояния кнопки был применен [ранее мной реализованный](https://github.com/JV4K/button-debounce-routine) модуль, представляющий собой программный фильтр на основе интегратора с триггером.
Также был реализован небольшой модуль (press_detection.h/.c), который детектриует короткое и длинное нажатие.
### Светодиод
Для моргания светодиода реализован модуль, работа которого представляет собой программный таймер, генерирующий на вход светодиода ШИМ с заполнением 50% заданной частоты.
### Программная реализация машины состояний
Машина состояний также вызывается с частотой 100Гц после обновления значений времени и состояния кнопок, чтобы оперировать актуальными данными. По сути это один SWITCH-statement, в котором в зависимости от режима обрабатываются состояния кнопок, и при необходимости устанавливается текущее время или время будильника, а также запускается моргание светодиода.

В режимах установки текущего времени и времени срабатывания будильника короткое нажатие кнопок инкрементирует минуты или часы (в зависимости от кнопки), а по длинному нажатию время прибавляется быстро (по 10 в секунду). При этом учтен формат времени BCD, использующемся в RTC на борту МК. Данный формат предполагает отображение десятичного числа в формате HEX. Т.е после 0х09 идет не 0х0A, а 0x10. Данный момент тоже был учтен, и написана функция инкремента под этот формат с ограничением 0x23 для часов и 0x59 для минут.

Учтено также:
- Зажатие кнопки, начавшееся в предыдущем режиме не влияет на текущий режим (нужно вначале отпустить кнопку, потом уже производить необходимые действия)
- Для выхода из режимов установки времени и будильника используется зажатие двух кнопок, что не всегда удается сделать синхронно между вызовами машины (10 мс), так что был добавлен таймаут для удобства пользования

"Моргание будильника" заканчивается по истечении 10 секунд, либо же после любого действия пользователя с кнопками.

Ход разработки можно отследить по коммитам в данном репозитории.

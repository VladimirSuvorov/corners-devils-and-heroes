# Corners

![Win](https://user-images.githubusercontent.com/32199420/166565617-40da829b-4f04-4175-907e-822f36d52b35.PNG)

# Как запустить?
Есть, как минимум, 2 варианта:

1) Через командную строку: `cd <в какую-нибудь папку>`, а потом `git clone https://github.com/VladimirSuvorov/corners-devils-and-heroes.git --recursive`.
2) В Visual Studio: при открытии студии (в окне создания/откытия проекта), выбрать "Клонирование репозитория" и ввести то же расположение репозитория `https://github.com/VladimirSuvorov/corners-devils-and-heroes.git`.

В студии всё может запуститься "из коробки". Через командную строку, потребуются CMake и - очевидно - Git. 

Для случая командной строки - см. `scripts/bats/RebuildRelease` (или `RebuildDebug`): по факту, содержание этих пакетных скриптов - просто запуск CMake и удаление некоторых файлов и папок. В случае со скриптами, готовое приложение будет размещено в подпапке проекта `/bin`.

В собранном виде, приложение выглядит примерно так (см. видео ниже):

https://user-images.githubusercontent.com/32199420/166568352-34f9d56c-b7ed-4bb0-8072-dc6199d6b791.mp4

(Видео записалось криво, поэтому слева есть рябь из мелких чёрточек, которых в игре нет.)

Зависимости, необходимые для запуска: 
[glfw3](https://github.com/glfw/glfw),
[glbinding](https://github.com/cginternals/glbinding),
[glm](https://github.com/g-truc/glm),
[gsl](https://github.com/microsoft/GSL),
[stb_image](https://github.com/nothings/stb/blob/master/stb_image.h).

При нормальных условиях, будучи прописанными в качестве сабмодулей git-а, **должны сами подгрузиться** прямиком в подпапку проекта `/externals`  (кроме stb_image, который вручную помещён в папке `/externals`).

Для запуска самого приложения нужны 2 файла: corners.png и DefaultShaderProgram.shader - они должны быть в одном месте с приложением. При нормальных условиях, **CMake сам их туда добавит**: в "ручном режиме" - через `install` в соответствующую подпапку `/bin`, в VisualStudio - через generator expressions в `add_custom_command`. 

## Как в это играть
В духе классических стратегий: клик на "юните" - клик на месте, куда бы ему пойти. Юниты ходят только на одну клетку С-З-Ю-В. Если кликнуть на другой юнит - выберется уже он. При клике на что-то, куда нельзя пойти - хода не произойдёт, а выбор будет сброшен (кроме случая, когда кликнут другой юнит - в таком случае уже он будет выбран). Клик на сам выбранный юнит - сбрасывает выбор. Все смайлики послушно и добродушно телепортируются куда надо. Прикрутить красивое выделение выбранных персонажей не успел, поэтому придётся без него. Правила в духе уголков обыконовенных. Возможны 3 исхода: победа смайликов, победа чертят и ничья, произошедшая из-за того, что один игрок зажал фигуры другого, что второму не осталось ходов. Игра добрая: из-за недоработок ИИ, черти яростно тупят и не очень-то стремятся захватить позиции противника, честно отравляя игроку жизнь и мешая побеждать (бесят, оправдывая свой облик и аццкое происхождение). Смайлики тоже могут "тупить", но это опять же из-за чертей: поток ИИ может тормозить принятие решения, поэтому ход ИИ может затянуться на несколько секунд или десятков (но в последних сборках такого не было). Меню и даже банальный freetype не прикручены: вся графика - спрайты, слепленные в один динамический меш. Перезапуска также нет. Закрывается по ESC. 


## В порядке небольшого FAQ по коду.
``` Хотели как лучше, а получилось как всегда. (C) Виктор Черномырдин ```
Код был написан - как и полагается тестовому заданию "на неделю" - "на коленке", по методологии "тяп-ляп и в продакшн". Чисто из-за временных ограничений. Разумеется, автор старался поаккуратнее, но недельный формат одноразового поделия - увы - никак не предполагает написания "промышленного кода", предполагаемого к многолетней поддержке. Между "сделать неэстетично" и "вообще не сделать" выбор не особо богатый. А "сделать хорошо" это не на неделю. Короче - получилось хорошо ровно настолько, насколько это возможно за неделю. И лучше не упоминать, что "продуманные архитектура и структура проекта" это не про недельнуые поделки.

В первую очередь, пришлось пренебречь namespace-ами: в микропроекте, который пишется быстро, аккуратное раскладываение сущностей по пространствам имён только мешает: перманентное переделывание создаёт предпосылки лишь для того, чтобы о них постоянно "спотыкаться". Поэтому их нет вообще. 

Оптимизация - также стала безвинной жертвой этого проекта. Не то, чтобы всё "жутко тормозит" - просто она была вообще не в приоритете. Тем не менее, даже мой домашний ПК с 2ГБ DDR2 и Intel Core 2 DUO не умирает, а стоически сносит данную игру - притом - при запущенной студии - 2022-й.

ИИ. По праву назван мной "Crazy Mad Insane AI". Как он работает - загадка. Выиграть не пытается (не амбициозен), но активно путается у игрока под ногами и мешает ему  пройти и выиграть. Являет собой чудовищную алхимическую смесь из дизайнерских наркотиков, алгоритмов A*, minimax и рандома. 

Возможно, возникнет вопрос: "а где все коммиты?". Вопрос вполне логичный. Просто, разработка велась в другом репозитории и - дабы скрыть "богохульные непотребства" творимые там от очей "святого престола" - пришлось пойти на "резет кармы" путём перезалива в другой репозиторий. Ибо методология разработки "тяп-ляп за неделю" не предрасполагает к аккуратным микрокоммитам, структурированию веток репозитория и т.п., а скорее подталкивает к потоку сознания, игнорирующему основное преимущество Git-а над SVN-ом - ветвление. 

Ну, а теперь к фичам.

1) Графон и анимация. Не модный, зато олдскульный: пиксельно-покадровый, ламповый. Увы, не дизайнер я, другого художества сотворить вне моих скромных компетенций. Мигает, изображая "вселенское добро" противостоящее "вселенскому злу". Кроме того, юниты умеют "телепортироваться" засчёт интерполяции цветофильтра и дополнительного спрайта отображающего предыдущую позицию.
2) ИИ. Не самый безнадёжный для недельной поделки. 
3) Графика в отдельном потоке: ничто так не отвлекает от умственной отсталости ИИ, как анимация, которая проигрывается несмотря на любые тормоза. 
4) В последних версиях ИИ переработан, использует многопоточные версии минимакса и подбора альтернативных действий (в котором задействован поиск пути, что может влечь тормоза), поэтому может вообще не тормозить - даже на двух ядрах с двумя ГБ памяти.
5) Возможно было что-то ещё, но под вечер я это забыл...)





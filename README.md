iqScreenSpy
===========

Программа для автоматического сохранения записи с экранов ОС Solaris.

Установка iqScreenSpy
-------------------
Для установки iqScreenSpy можно воспользоваться либо скриптом `/sintez/sintez/bin/iqSetup`, либо установить вручную, для этого необходимо выполнить следующие действия:
* Устанавить OpenCSW и настроить его для работы с локальным репозиторием
~~~~~~{bash}
    su
    cd /
    pkgadd -d http://web/opencsw/pkgutil-i386.pkg
    /usr/sfw/bin/wget http://web/iq/pkgutil.conf_localmirror.patch
    gpatch -p1 < pkgutil.conf_localmirror.patch
    rm pkgutil.conf_localmirror.patch
    /opt/csw/bin/pkgutil -U
~~~~~~
* Установить библиотеки, от которых зависит iqLibs
~~~~~~{bash}
    su
    /opt/csw/bin/pkgutil -iy libstdc++6 libicui18n52 mesalibs libxcb libxrender fontconfig dbus gperf bison flex libx264_133
~~~~~~
* Скачать и развернуть iqLibs (Qt-5.4.0, ffmpeg и другие)
~~~~~~{bash}
    su
    cd /
    /usr/sfw/bin/wget http://web/iq/iq.tar.gz
    gunzip -c iq.tar.gz | tar xf -
    rm iq.tar.gz
~~~~~~
* Установить зависимости iqScreenSpy
~~~~~~
    su
    /opt/csw/bin/pkgutil -iy grat
~~~~~~
* Скачать iqScreenSpy
~~~~~~{bash}
    su sintez
    cd /sintez/sintez/bin/
    /usr/sfw/bin/wget http://web/iq/iqScreenSpy
    chown sintez:syn iqScreenSpy #Если забыли выйти из под root и скачали файл из под него
    chmod 755 iqSceenSpy
~~~~~~
* Создать папку для записи
~~~~~~
    su
    mkdir /sd/spyscreens/
    chown -R sintez:syn /sd/spyscreens/
~~~~~~
* На целевом рабочем месте запустить и закрыть iqScreenSpy командой */sintez/sintez/bin/iqScreenSpy*. После этого будет создан файл */sintez/sintez/.config/itQuasar/iqScreenSpy.conf* с настройками.
* Отредактировать настройки в файле */sintez/sintez/.config/itQuasar/iqScreeSpy.conf*.
* При необходимости добавить iqScrenSpy в автоматический запуск, через startfile. Для этого необходимо отредактировать файл */sintez/sintez/startfile_atd* (для рабочих мест) или */sintez/sintez/startfile_stuk* (для СТУК), добавив в него строки:
~~~~~~~~{bash}    
    #------------iqScreenSpy---------------
    cd $SINTEZ_HOME/bin
    LD_LIBRARY_PATH="" QT_XKB_CONFIG_ROOT=/usr/openwin/lib/X11/xkb $uexec "./iqScreenSpy > /dev/null 2>&1 &"
~~~~~~~~
* Перезагрузить все процессы на рабочем месте.

Настройка iqScreenSpy
-------------------
Все настройки iqScreenSpy хранятся в файле */sintez/sintez/.config/itQuasar/iqScreenSpy.conf*. Настройки можно редактировать в простом текстовом редакторе. После изменения настроек, необходимо перезапустить iqScreepSpy.

###Описание настроек
* **[General]** - основные настройки.
* *outputDir* - папка, в которую будет производиться запись видео.
* **[ArchiveParams]** - параметры архивации старых видео.
* *commpressEnabled* - признак того, что будет использоваться сжатие для архивации старых записей.
* *tarBinPath* - полный путь к GNU tar.
* *uncompressedStorageTime* - время в млсек в течении которого видео будет храниться без архивации.
* **[FFmpegParams]** - параметры работы ffmpeg.
* *extraLdLibraryPath* - путь который будет добавлен к LD_LIBRARY_PATH для поиска библиотек используемых ffmpeg.
* *ffmpegBinPath* - полный путь к ffmpeg.
* *maximumFps* - максимальное колличество кадров в секунду в результате работы ffmpeg.
* *maximumThreads* - максимальное колличество потоков, которое может использовать ffmpeg для сжатия видео.
* *outputFileExtension* - расширение выходного видое.
* *vcodecParams* - параметры, передаваемые ffmpeg с ключем -vcodec.
* **[RotateParams]** - параметры ротации видео.
* *storageTime* - время в млсек в, течении которого будет храниться видео.

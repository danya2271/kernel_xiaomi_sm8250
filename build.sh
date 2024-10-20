#!/bin/bash

# Начало отсчета времени выполнения скрипта
start_time=$(date +%s)

# Удаление каталога "out", если он существует
rm -rf out

# Основной каталог
MAINPATH=/home/timisong # измените, если необходимо

# Каталог ядра
KERNEL_DIR=$MAINPATH/kernel

# Каталоги компиляторов
SNAPDRAGON_CLANG_DIR=$KERNEL_DIR/snapdragon-clang
ANDROID_PREBUILTS_GCC_ARM_DIR=$KERNEL_DIR/android_prebuilts_gcc_linux-x86_arm_arm-linux-androideabi-4.9
ANDROID_PREBUILTS_GCC_AARCH64_DIR=$KERNEL_DIR/android_prebuilts_gcc_linux-x86_aarch64_aarch64-linux-android-4.9

# Проверка и клонирование, если необходимо
check_and_clone() {
    local dir=$1
    local repo=$2

    if [ ! -d "$dir" ]; then
        echo "Папка $dir не существует. Клонирование $repo."
        git clone $repo $dir
    fi
}

# Клонирование инструментов компиляции, если они не существуют
check_and_clone $SNAPDRAGON_CLANG_DIR https://gitlab.com/VoidUI/snapdragon-clang
check_and_clone $ANDROID_PREBUILTS_GCC_ARM_DIR https://github.com/LineageOS/android_prebuilts_gcc_linux-x86_arm_arm-linux-androideabi-4.9
check_and_clone $ANDROID_PREBUILTS_GCC_AARCH64_DIR https://github.com/LineageOS/android_prebuilts_gcc_linux-x86_aarch64_aarch64-linux-android-4.9

# Установка переменных PATH
PATH=$SNAPDRAGON_CLANG_DIR/bin:$ANDROID_PREBUILTS_GCC_AARCH64_DIR/bin:$ANDROID_PREBUILTS_GCC_ARM_DIR/bin:$PATH
export PATH
export ARCH=arm64

# Каталог для сборки MagicTime
MAGIC_TIME_DIR="$KERNEL_DIR/MagicTime"

# Создание каталога MagicTime, если его нет
if [ ! -d "$MAGIC_TIME_DIR" ]; then
    mkdir -p "$MAGIC_TIME_DIR"
    
    # Проверка и клонирование Anykernel, если MagicTime не существует
    if [ ! -d "$MAGIC_TIME_DIR/Anykernel" ]; then
        git clone https://github.com/decoder-dev/Anykernel.git "$MAGIC_TIME_DIR/Anykernel"
        
        # Перемещение всех файлов из Anykernel в MagicTime
        mv "$MAGIC_TIME_DIR/Anykernel/"* "$MAGIC_TIME_DIR/"
        
        # Удаление папки Anykernel
        rm -rf "$MAGIC_TIME_DIR/Anykernel"
    fi
else
    # Если папка MagicTime существует, проверить наличие .git и удалить, если есть
    if [ -d "$MAGIC_TIME_DIR/.git" ]; then
        rm -rf "$MAGIC_TIME_DIR/.git"
    fi
fi

# Экспорт переменных среды
export IMGPATH="$MAGIC_TIME_DIR/Image"
export DTBPATH="$MAGIC_TIME_DIR/dtb"
export CLANG_TRIPLE="aarch64-linux-gnu-"
export CROSS_COMPILE="aarch64-linux-gnu-"
export CROSS_COMPILE_ARM32="arm-linux-gnueabi-"
export KBUILD_BUILD_USER="TIMISONG"
export KBUILD_BUILD_HOST="timisong-dev"
export MODEL="alioth"

# Запись времени сборки
MAGIC_BUILD_DATE=$(date '+%Y-%m-%d_%H-%M-%S')

# Каталог для результатов сборки
output_dir=out

# Конфигурация ядра
make O="$output_dir" \
            alioth_defconfig

# Проверка успешности конфигурации
if [ $? -eq 0 ]; then
    # Компиляция ядра
    make -kj$(nproc --all) \
                O="$output_dir" \
                CC="ccache clang" \
                HOSTCC=gcc \
                LD=ld.lld \
                AS=llvm-as \
                AR=llvm-ar \
                NM=llvm-nm \
                OBJCOPY=llvm-objcopy \
                OBJDUMP=llvm-objdump \
                STRIP=llvm-strip \
                LLVM=1 \
                LLVM_IAS=1 \
                V=$VERBOSE 2>&1 | tee error.log
                
    # Проверка успешности сборки
    if [ $? -eq 0 ]; then
        message="\e[32mОбщее время выполнения: $elapsed_time секунд\e[0m"
    else
        message="\e[31mОшибка: Сборка завершилась с ошибкой\e[0m"
    fi
else
    message="\e[31mОшибка: Не удалось настроить конфигурацию ядра\e[0m"
fi

# Предполагается, что переменная DTS установлена ранее в скрипте
find $DTS -name '*.dtb' -exec cat {} + > $DTBPATH
find $DTS -name 'Image' -exec cat {} + > $IMGPATH

# Перемещение в каталог MagicTime и создание архива
cd "$MAGIC_TIME_DIR"
7z a -mx9 MagicTime-$MODEL-$MAGIC_BUILD_DATE.zip * -x!*.zip

# Завершение отсчета времени выполнения скрипта
end_time=$(date +%s)
elapsed_time=$((end_time - start_time))

# Вывод сообщения о завершении выполнения скрипта
echo -e "$message"
echo "Total execution time: $elapsed_time seconds"
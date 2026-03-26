# Makefile для mOS

# ----------------------------
# 1. Находим все исходники
C_SOURCES := $(shell find . -name '*.c')
ASM_SOURCES := $(shell find . -name '*.asm')

# ----------------------------
# 2. Генерируем объектные файлы
C_OBJS := $(C_SOURCES:.c=.o)
ASM_OBJS := $(ASM_SOURCES:.asm=.o)

# ----------------------------
# 3. Указываем точку входа
ENTRY_OBJ := ./entry.o

# ----------------------------
# 4. Полный список объектов для линковки (ENTRY первым)
OBJ := $(ENTRY_OBJ) $(filter-out $(ENTRY_OBJ), $(C_OBJS) $(ASM_OBJS))

# ----------------------------
# 5. Флаги
CFLAGS = -m32 -ffreestanding -Wall -Wextra -I.
NASMFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

# ----------------------------
# 6. Цель сборки ядра
kernel: $(OBJ)
	ld $(LDFLAGS) -o $@ $^

# ----------------------------
# 7. Правила сборки
%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

%.o: %.asm
	nasm $(NASMFLAGS) $< -o $@

# ----------------------------
# 8. Очистка
clean:
	find . -name "*.o" -delete
	rm -f kernel

# ----------------------------
# 9. Запуск в QEMU
run: kernel
	qemu-system-x86_64 -s -kernel kernel --enable-kvm -m 512 -monitor stdio -hda hdd.img 

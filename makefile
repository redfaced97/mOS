# 1. Находим ВСЕ исходники
C_SOURCES := $(shell find . -name '*.c')
ASM_SOURCES := $(shell find . -name '*.asm')

# 2. Генерируем полный список объектных файлов
ALL_OBJ := $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.o)

# 3. Указываем путь к вашему входному файлу (замените путь, если он в подпапке)
ENTRY_OBJ := ./entry.o

# 4. Формируем список для линковки: ENTRY первым, остальные — следом (без дубликатов)
# Функция filter-out удалит entry.o из общего списка, чтобы он не встречался дважды
OBJ := $(ENTRY_OBJ) $(filter-out $(ENTRY_OBJ), $(ALL_OBJ))

# Флаги
CFLAGS = -m32 -ffreestanding -c -I.
LDFLAGS = -m elf_i386 -T linker.ld

kernel: ${OBJ}
	   ld ${LDFLAGS} -o $@ $^

%.o: %.c
	   gcc ${CFLAGS} $< -o $@

%.o: %.asm
	   nasm -f elf32 $< -o $@

clean:
	   find . -name "*.o" -delete
	   rm -f kernel

run: kernel
	qemu-system-x86_64 -kernel kernel --enable-kvm -m 512 -monitor stdio

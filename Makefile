# FILE: bootloader/Makefile
TARGET = bootloader.efi
SRC = bootloader.cpp
OBJ = $(SRC:.cpp=.o)
CXX = x86_64-efi-g++
CXXFLAGS = -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -Wall -Wextra
LDFLAGS = -nostdlib -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib -l:libgnuefi.a -l:libefi.a

all: $(TARGET)

$(TARGET): $(OBJ)
    $(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
    $(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
/*
 * Copyright (c) 2015 Sergi Granell (xerpi)
 */

#include "elf.h"
#include <stdlib.h>

int elf_read_ehdr(FILE *fp, Elf32_Ehdr *ehdr)
{
	fseek(fp, 0, SEEK_SET);
	size_t n = fread(ehdr, 1, sizeof(*ehdr), fp);

	if (n != sizeof(*ehdr)) {
		fprintf(stderr, "Could not read from file\n");
		return 0;
	}

	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
	    ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
	    ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
	    ehdr->e_ident[EI_MAG3] != ELFMAG3) {
		fprintf(stderr, "Not an ELF file\n");
		return 0;
	}

	if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
		fprintf(stderr, "Not a 32 bit ELF\n");
		return 0;
	}

	if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		fprintf(stderr, "Not a little endian ELF\n");
		return 0;
	}

	if (ehdr->e_type != ET_SCE_RELEXEC) {
		fprintf(stderr, "Not an executable SCE_RELEXEC ELF\n");
		return 0;
	}

	if (ehdr->e_machine != EM_ARM) {
		fprintf(stderr, "Not an ARM ELF\n");
		return 0;
	}

	return 1;
}

int elf_load_phdr(FILE *fp, const Elf32_Ehdr *ehdr, Elf32_Phdr **phdr)
{
	size_t total = ehdr->e_phnum * ehdr->e_phentsize;
	*phdr = malloc(total);

	fseek(fp, ehdr->e_phoff, SEEK_SET);

	size_t n = fread(*phdr, 1, total, fp);

	if (n != total) {
		fprintf(stderr, "Could not read Program Headers\n");
		free(*phdr);
		return 0;
	}

	return 1;
}

int elf_load_shdr(FILE *fp, const Elf32_Ehdr *ehdr, Elf32_Shdr **shdr)
{
	size_t total = ehdr->e_shnum * ehdr->e_shentsize;
	*shdr = malloc(total);

	fseek(fp, ehdr->e_shoff, SEEK_SET);

	size_t n = fread(*shdr, 1, total, fp);

	if (n != total) {
		fprintf(stderr, "Could not read Section Headers\n");
		free(*shdr);
		return 0;
	}

	return 1;
}

void elf_free_phdr(Elf32_Phdr **phdr)
{
	if (phdr) {
		free(*phdr);
		*phdr = NULL;
	}
}

void elf_free_shdr(Elf32_Shdr **shdr)
{
	if (shdr) {
		free(*shdr);
		*shdr = NULL;
	}
}

char *elf_load_shstrtab(FILE *fp, const Elf32_Ehdr *ehdr, const Elf32_Shdr *shdr)
{
	// Get pointer to the String Table Section (.shstrtab)
	const Elf32_Shdr *shstrtab_shdr;
	shstrtab_shdr = &shdr[ehdr->e_shstrndx];

	// Load entire String Table Section
	char *shstrtab = malloc(shstrtab_shdr->sh_size * sizeof(char));

	fseek(fp, shstrtab_shdr->sh_offset, SEEK_SET);
	fread(shstrtab, 1, shstrtab_shdr->sh_size * sizeof(char), fp);

	return shstrtab;
}

void elf_print_ehdr(const Elf32_Ehdr *ehdr)
{
	printf(
		"ELF Header:\n"
		"  Class:                             0x%X\n"
		"  Data:                              0x%X\n"
		"  Version:                           0x%X\n"
		"  OS/ABI:                            0x%X\n"
		"  ABI Version:                       0x%X\n"
		"  Type:                              0x%X\n"
		"  Machine:                           0x%X\n"
		"  Version:                           0x%X\n"
		"  Entry point address:               0x%X\n"
		"  Start of program headers:          0x%X\n"
		"  Start of section headers:          0x%X\n"
		"  Flags:                             0x%X\n"
		"  Size of this header:               0x%X (bytes)\n"
		"  Size of program headers:           0x%X (bytes)\n"
		"  Number of program headers:         0x%X\n"
		"  Size of section headers:           0x%X (bytes)\n"
		"  Number of section headers:         0x%X\n"
		"  Section header string table index: 0x%X\n\n",
	ehdr->e_ident[EI_CLASS], ehdr->e_ident[EI_DATA],
	ehdr->e_ident[EI_VERSION], ehdr->e_ident[EI_OSABI],
	ehdr->e_ident[EI_ABIVERSION], ehdr->e_type,
	ehdr->e_machine, ehdr->e_version, ehdr->e_entry,
	ehdr->e_phoff, ehdr->e_shoff, ehdr->e_flags,
	ehdr->e_ehsize, ehdr->e_phentsize, ehdr->e_phnum,
	ehdr->e_shentsize, ehdr->e_shnum, ehdr->e_shstrndx);
}

void elf_print_phdr(const Elf32_Phdr *phdr)
{
	printf(
		"  Type:                              0x%X\n"
		"  File offset:                       0x%X\n"
		"  Virtual address:                   0x%X\n"
		"  Physical address:                  0x%X\n"
		"  File size:                         0x%X (bytes)\n"
		"  Memory size:                       0x%X (bytes)\n"
		"  Flags:                             0x%X\n"
		"  Alignment:                         0x%X\n\n",
	phdr->p_type, phdr->p_offset, phdr->p_vaddr,
	phdr->p_paddr, phdr->p_filesz, phdr->p_memsz,
	phdr->p_flags, phdr->p_align);
}

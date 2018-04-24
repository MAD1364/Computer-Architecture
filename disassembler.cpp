#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <iomanip>

using namespace std;

const int REGISTERS = 32;
const string regNames[REGISTERS] = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3",
                                    "$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
                                    "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
                                    "$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"};

uint16_t convertToBEHalfWord(uint8_t* half)
{
    return ((half[0] << 8) | half[1]);
}

uint32_t convertToBEWord(uint8_t* word)
{
    return ((word[0] << 24) | (word[1] << 16) | (word[2] << 8) | word[3]);
}

void disassemble(int pc, uint32_t instr, ostream& assembly)
{
  uint32_t opcode= instr >> 26;                    // opcode field
  uint32_t rs, rt, rd;                             // register specifiers
  uint32_t shamt= ( instr >> 6 ) & 0x1F;           // shift amount (R-type)
  uint32_t funct= ( instr ) & 0x3F;                // funct field (R-type)
  uint32_t u_immediate= ~( instr & 0xFFFF ) + 1;   // unsigned version of immediate (I-type)
  int32_t s_immediate= int32_t( int16_t ( ~u_immediate + 1 ) );
                                                   // signed version of immediate (I-type)
  uint32_t addr= ( instr ) & 0x3FFFFFF;            // jump address offset field (J-type)

  rs = ( instr >> 21 ) & 0x1F;
  rt = ( instr >> 16 ) & 0x1F;
  rd = ( instr >> 11 ) & 0x1F;

  cout << hex << setw(8) << pc << ": ";
  switch(opcode)
  {
    case 0x00:
      switch(funct)
      {
        case 0x00: cout << "sll " << regNames[rd] << ", " << regNames[rs] <<
                   ", " << dec << shamt;
		   assembly << "sll " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt; break;

        case 0x02: cout << "srl " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt];
		   assembly << "sll " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;

        case 0x3: cout << "sra " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt;
         	  assembly << "sra " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt; break;

        case 0x8: cout << "jr " << regNames[rs];
                  assembly << "jr " << regNames[rs]; break;

	case 0xc: cout << "syscall ";
		  assembly << "syscall "; break;

        case 0x10: cout << "mfhi " << regNames[rd];
		   assembly << "mfhi " << regNames[rd]; break;

        case 0x12: cout << "mflo " << regNames[rd];
	           assembly << "mflo " << regNames[rd]; break;

        case 0x18: cout << "mult " << regNames[rs] << ", " << regNames[rt];
                   assembly << "mult " << regNames[rs] << ", " << regNames[rt]; break;

        case 0x1a: cout << "div " << regNames[rs] << ", " << regNames[rt];
                   assembly << "div " << regNames[rs] << ", " << regNames[rt]; break;

        case 0x20: cout << "add " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt];
		   assembly << "add " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;

        case 0x22: cout << "sub " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt];
		   assembly << "sub " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;

        case 0x21: cout << "addu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt];
		   assembly << "addu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;

        case 0x23: cout << "subu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt];
		   assembly << "subu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;

        case 0x24: cout << "and " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt];
		   assembly << "and " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;

        case 0x27: cout << "nor " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt];
		   assembly << "nor " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;

        case 0x2a: cout << "slt " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt];
		   assembly << "slt " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;

        case 0x2b: cout << "sltu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt];
		   assembly << "sltu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;

        default: cout << "unimplemented";
      }
      break;
    case 0x2: cout << "j " << hex << ((pc + 4) & 0xf0000000) + addr * 4;
	      assembly << "j " << hex << ((pc + 4) & 0xf0000000) + addr * 4; break;

    case 0x3: cout << "jal " << hex << ( ( pc + 4 ) & 0xf0000000 ) + addr * 4;
              assembly << "jal " << hex << ((pc + 4) & 0xf0000000) + addr * 4; break;

    case 0x4: cout << "beq " << regNames[rs] << ", " << regNames[rt] << ", " << hex << ( pc + 4 ) + ( s_immediate * 4 );
	      assembly << "beq " << regNames[rs] << ", " << regNames[rt] << ", " << hex << (pc + 4) + (s_immediate * 4); break;

    case 0x5: cout << "bne " << regNames[rs] << ", " << regNames[rt] << ", " << hex << ( pc + 4 ) + ( s_immediate * 4 );
	      assembly << "bne " << regNames[rs] << ", " << regNames[rt] << ", " << hex << (pc + 4) + (s_immediate * 4); break;

    case 0x8: cout << "addi " << regNames[rt] << ", " << regNames[rs] << ", " << dec << s_immediate;
	      assembly << "addi " << regNames[rt] << ", " << regNames[rs] << ", " << dec << s_immediate; break;

    case 0x9: cout << "addiu " << regNames[rt] << ", " << regNames[rs] << ", " << dec << s_immediate;
	      assembly << "addiu " << regNames[rt] << ", " << regNames[rs] << ", " << dec << s_immediate; break;

    case 0xa: cout << "slti " << regNames[rt] << ", " << regNames[rs] << dec << s_immediate;
	      assembly << "slti " << regNames[rt] << ", " << regNames[rs] << dec << s_immediate; break;

    case 0xb: cout << "sltiu " << regNames[rt] << ", " << regNames[rs] << dec << u_immediate;
	      assembly << "sltiu " << regNames[rt] << ", " << regNames[rs] << dec << u_immediate; break;

    case 0xc: cout << "andi " << regNames[rt] << ", " << regNames[rs] << ", " << dec << s_immediate;
	      assembly << "andi " << regNames[rt] << ", " << regNames[rs] << ", " << dec << s_immediate; break;

    case 0xf: cout << "lui " << regNames[rt] << ", " << dec << s_immediate;
	      assembly << "lui " << regNames[rt] << ", " << dec << s_immediate; break;

    case 0x1a: cout << "trap " << hex << addr;
	       assembly << "trap " << hex << addr; break;

    case 0x20: cout << "lb " << regNames[rt] << ", " << dec << s_immediate;
	       assembly << "lb " << regNames[rt] << ", " << dec << s_immediate; break;

    case 0x23: cout << "lw " << regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")";
	       assembly << "lw " << regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")"; break;

    case 0x24: cout << "lbu " << regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")";
	       assembly << "lbu " << regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")"; break;

    case 0x25: cout << "lhu " << regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")";
	       assembly << "lhu " << regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")"; break;

    case 0x28: cout << "sb " << regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")";
	       assembly << "sb " << regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")"; break;

    case 0x2b: cout << "sw " <<regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")";
	       assembly << "sw " << regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")"; break;

    case 0x38: cout << "sc " << regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")";
	       assembly << "sc " << regNames[rt] << ", " << dec << s_immediate << "(" << regNames[rs] << ")"; break;

    default: cout << "unimplemented";
  }

  cout << endl;
  assembly << endl;
}

int main(int argc, char* argv[])
{
    ifstream object_file;   //Input File Stream object to read the binary file
    ofstream assembly_file;//Output File Stream object to write the disassembled instructions to
    int object_file_type,   //Variables used to represent the information in an ELF header
        architecture,
        object_file_version,
   	entry_point,
     	program_header_offset,
 	section_header_offset,
	flags,
	elf_header_size,
	program_header_entry_size,
	entries_in_program_header,
	section_header_entry_size,
	entries_in_section_header,
	string_section_index;
    uint8_t half[2];       //Array used to store two consecutive bytes read from the binary file
    uint8_t word[4];       //Array used to store four consecutive bytes read from the binary file
    char elf_file[16];     //Character array used to store the first 16 bytes from the object file

    object_file.open(argv[1], ios::binary | ios::in);   //Open the object file as a binary input file (To read from)

    if(!object_file)
       cout << "Failed to open the file " << argv[1] << endl;
    else
       cout << argv[1] << " opened successfully." << endl;


    object_file.read(elf_file, 16);
    cout << elf_file << "After ELF" << endl;

    object_file.read((char*)&half, 2);
    object_file_type = convertToBEHalfWord(half);
    cout << "Object File Type: " << object_file_type << endl;

    object_file.read((char*)&half, 2);
    architecture = convertToBEHalfWord(half);
    cout << "Required Architecture: " << architecture << endl;

    object_file.read((char*)&word, 4);
    object_file_version = convertToBEWord(word);
    cout << "Object File Version: " << object_file_version << endl;

    object_file.read((char*)&word, 4);
    entry_point = convertToBEWord(word);
    cout << "Entry Point: " << entry_point << endl;
    //Virtual address to which system first transfers control.
    object_file.read((char*)&word, 4);
    program_header_offset = convertToBEWord(word);
    cout << "Program Header Offset: " << program_header_offset << endl;

    object_file.read((char*)&word, 4);
    section_header_offset = convertToBEWord(word);
    cout << "Section Header Offset: " << section_header_offset << endl;

    object_file.read((char*)&word, 4);
    flags = convertToBEWord(word);
    cout << "Processor-Specific Flags: " << flags << endl;

    object_file.read((char*)&half, 2);
    elf_header_size = convertToBEHalfWord(half);
    cout << "Size of ELF Header in bytes: " << elf_header_size << endl;

    object_file.read((char*)&half, 2);
    program_header_entry_size = convertToBEHalfWord(half);
    cout << "Program Header Entry size (bytes): " << program_header_entry_size << endl;

    object_file.read((char*)&half, 2);
    entries_in_program_header = convertToBEHalfWord(half);
    cout << "Number of Entries in Program Header: " << entries_in_program_header << endl;

    object_file.read((char*)&half, 2);
    section_header_entry_size = convertToBEHalfWord(half);
    cout << "Size of Section Header: "
         << section_header_entry_size << endl;
    //Entry in Section Header Table, all of which are the same size.
    object_file.read((char*)&half, 2);
    entries_in_section_header = convertToBEHalfWord(half);
    cout << "Section Header Table Entries: " << entries_in_section_header << endl;
    //Number of entries. (Note: The product of the previous two values gives the size of the section header table.
    object_file.read((char*)&half, 2);
    string_section_index = convertToBEHalfWord(half);
    cout << "String Section Index: " << string_section_index << endl;
    //String Section contains null-terminated strings used trhoughout the program.

    object_file.seekg(0, ios::beg); //Move file reading pointer to the begging of the binary file
    int count = 0,                  //Variable used to count the number of bytes in the file
        bytes = 0;                  //Variable used to read the binary file one byte at a time
    while(object_file.read((char*)&bytes, 1))
      count++;

    cout << "Total Bytes in " << argv[1] << ": " << count << endl;

    object_file.clear(); //Clear the EOF bit that is set after the file has been read to the end.

    int section_name,       //Index into the section header string table
	section_type,       //Category of section's contents and semantics
	section_flags,      //Describe miscellaneous attributes
	section_address,    //Address of first byte of section
	section_offset,     //Byte offset of the section from the beggining of the file
	section_size,       //Size of the section in bytes if(!SHT_NOBITS) -> sh_size
	section_link,       //Index Link into the Section Header Table
	section_info,       //Information whose interpretation depends on the section type
	section_address_alignment, //Address alignment constraint section_address must be congruent to
				   //0 modulo section_address_alignment
	section_entry_size, //Size of entries in section such as a symbol table
        text_offset,
	text_size,
	data_offset,
	data_size,
	string_offset,
	string_size;

    object_file.seekg(section_header_offset, ios::beg);

    for (int i = 0; i < entries_in_section_header; i++)
    {
	object_file.read((char*)&word, 4);
	section_name = convertToBEWord(word);
	cout << "Section Name: " << section_name << endl; //Index into string table for section name

	object_file.read((char*)&word, 4);
	section_type = convertToBEWord(word);
	cout << "Section Type: " << section_type << endl;

	object_file.read((char*)&word, 4);
	section_flags = convertToBEWord(word);
	cout << "Section Flags: " << section_flags << endl;

	object_file.read((char*)&word, 4);
	section_address = convertToBEWord(word);
	cout << "Section Address: " << section_address << endl;

	object_file.read((char*)&word, 4);
	section_offset = convertToBEWord(word);
	cout << "Section Offset: " << section_offset << endl;

	object_file.read((char*)&word, 4);
	section_size = convertToBEWord(word);
	cout << "Section Size: " << section_size << endl;

	if (i == 1)
	{
	    text_offset = section_offset;
	    text_size = section_size;
	}

	if (i == 3)
	{
	    data_offset = section_offset;
	    data_size = section_size;
	}

	if (i == 9)
	{
	    string_offset = section_offset;
	    string_size = section_size;
	}

        object_file.read((char*)&word, 4);
	section_link = convertToBEWord(word);
	cout << "Section Link: " << section_link << endl;

	object_file.read((char*)&word, 4);
	section_info = convertToBEWord(word);
	cout << "Section Info: " << section_info << endl;

	object_file.read((char*)&word, 4);
	section_address_alignment = convertToBEWord(word);
	cout << "Section Address Alignment: " << section_address_alignment << endl;

	object_file.read((char*)&word, 4);
	section_entry_size = convertToBEWord(word);
	cout << "Section Entry Size: " << section_entry_size << endl << endl;
    }
    cin.get();
    if(object_file.eof())
	object_file.clear();

    string name;
    char byte;

    //Create assembly file based on the object file disassembled.
    string assembled_file = "assembled_" + string(argv[1]) + ".asm";	 //Create string representing name of output file
    assembly_file.open(assembled_file, ios::out);			 //Open output file
    if (!assembly_file)							 //Determine whether output file successfully opened
	cerr << "Error: Failed to open the output file." << endl;
    else
	cout << "Creating Assembly File..." << endl;

    //Start writing the assembly file with the data section
    assembly_file << setw(10) << ".data" << endl;

    int labels = 0,
	string_count = 0,
        data_count = 0;

    object_file.seekg(string_offset, ios::beg);
    for (int i = 0; i < string_size; i++)
    {
	object_file.read((char*)&byte, 1);
	if (byte == '\0')
	    labels++;
	cout << byte;
    }
    cout << labels << endl << endl;
    labels--;
    string_offset++;

    for (int i = 0; i < labels; i++)
    {
	if (object_file.eof())
	    object_file.clear();

	object_file.seekg(string_offset + string_count, ios::beg);

	do
	{
	    object_file.read((char*)&byte, 1);
	    if (byte != '\0')
		name += byte;
	    string_count++;
	}while(byte != '\0');

	assembly_file << name << ":" << '\t';

	if (name == "main")
	    break;
	else
	    name = "";

	if (object_file.eof())
	    object_file.clear();

	object_file.seekg(data_offset + data_count, ios::beg);

	do
	{
	    object_file.read((char*)&byte, 1);
	    cout << byte;
	    if (byte != '\0')
                name += byte;
	    data_count++;
	}while(byte != '\0');

	cout << name << endl;
	assembly_file << name << endl;
	name = "";
    }

    if (object_file.eof())
	object_file.clear();

    object_file.seekg(text_offset, ios::beg);
    uint32_t* instructions = new uint32_t[text_size/4];
    for (int i = 0; i < (text_size/4); i++)
    {
	if(!object_file.read((char*)&word, 4))
	{
	    cerr << "Error: Failed to read instructions from the file " << argv[1] << endl;
	}
	instructions[i] = convertToBEWord(word);
    }

    //Create text section (instructions) in the assembly file
    assembly_file << setw(10) << ".text" << endl;
    assembly_file << name << ":" << '\t';

    for (int i = 0; i < (text_size/4); i++)
    {
	if (i != 0)
	    assembly_file << '\t';
	disassemble(i * 4, instructions[i], assembly_file);
    }

    return 0;
}


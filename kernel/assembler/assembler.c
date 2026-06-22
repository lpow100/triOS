/*#include "assembler.h"
#include "syscalls.h"

jmp_buf de_stack_env;

/*
TODO - AT&T Syntax Features:
 * Local Labels
 * Numeric Labels
*

// error types are defined when multiple types of errors can occur in one function, otherwise just use 1 and 0

#define GENERAL_ERROR          1
#define LABEL_ERROR            2
#define NO_MEMORY_ERROR        3
#define INSTRUCTION_ERROR      4
#define BAD_STRING_ERROR       5
#define DATA_ENTRY_ERROR       6

typedef struct {
    uint64_t offset;
    char data[256];
} DataEntry;

typedef struct {
    char name[64];
    DataEntry dataEntries[8];
    uint8_t entryCount;
    uint32_t address;
} Label;

typedef struct {
    char mnemonic[32];  // "movl", "int", ".string", etc
    char arg1[64];      // "$4", "%eax", "$hello"
    char arg2[64];      // "%eax", empty if no second arg
} Instruction;

bool isLabelChar(char chr) {
    return (chr >= 'a' && chr <= 'z') ||
           (chr >= 'A' && chr <= 'Z') ||
           (chr >= '0' && chr <= '9') ||
           chr == '_' || chr == '.';
}

bool isLabelStartChar(char chr) {
    return (chr >= 'a' && chr <= 'z') ||
           (chr >= 'A' && chr <= 'Z') ||
           chr == '_' || chr == '.';
}

int makeLabelFromLine(char* line, char buffer[64]) {
    for (int i = 0; i < strlen(line); i++) {
        char chr = line[i];
        if (chr == ':') {
            buffer[i] = '\0';
            return 0;
        }
        if ((i == 0 && !isLabelStartChar(chr)) || (i != 0 && !isLabelChar(chr))) return 1;
        buffer[i] = chr;
    }
    return 1;
}

int addNewLabel(Label labels[256], uint8_t *label_count, char name[64], uint32_t address) {
    if (*label_count == 256) return 1;
    memcpy(labels[*label_count].name, name, strlen(name) + 1);
    labels[*label_count].address = address;
    (*label_count)++;
    return 0;
}

int addNewDataEntry(DataEntry entries[8], uint8_t *entry_count, char data[256], uint32_t address) {
    if (*entry_count == 8) return 1;
    memcpy(entries[*entry_count].data, data, strlen(data) + 1);
    entries[*entry_count].offset = address;
    (*entry_count)++;
    return 0;
}

int getLabelAddress(Label labels[256], uint8_t label_count, char name[64], uint32_t *value) {
    if (label_count == 0) return 1;
    for (uint8_t i = 0; i < label_count; i++) {
        if (strcmp(labels[i].name,name) == 0) {
            *value = labels[i].address;
            return 0;
        }
    }
    return 1;
}

int getString(const char* line, char* buff) {
    bool foundString = false;
    uint32_t stringStart = 0;
    for (int i = 0; i < strlen(line); i++) {
        char chr = line[i];

        if (chr == '"' && !foundString) { foundString = true; stringStart = i + 1; continue; }
        if (!foundString) continue;
        if (chr == '"') return 0;

        uint32_t stringIdx = i - stringStart;
        buff[stringIdx] = line[i];
    }

    return 1;
}

uint8_t getInstructionSize(const char* line) {
    char instruction[32];
    uint8_t instructionLen = 0;
    bool isImmediate = false;
    for (int i = 0; i < strlen(line); i++) {
        if (instructionLen > 0) {
            if (line[i] == ' ' || line[i] == '\t') continue;
            if (line[i] == '$') isImmediate = true;
            break;
        }
        if (line[i] == ' ' || line[i] == '\t') {
            instructionLen = i + 1; // add 1 because we start at 0 
            instruction[i] = '\0';
            continue;
        }
        instruction[i] = line[i];
    }
    if (strcmp(instruction, "movq") == 0) return isImmediate ? 7 : 3;
    else if (strcmp(instruction, "int") == 0) return 2;
    return 0;
}

struct LabelPassState {
    Label labels[256];
    uint8_t labelCount;
    int dataOffset;
    int textStart;
}

int findLabels(const char* assembly, int size, struct LabelPassState *state) {
    int lineStart = 0;
    int lineCount = 0;
    for (int idx = 0; idx < size; idx++) {
        char chr = assembly[idx];
        if (lineStart == idx && (chr == '\t' || chr == ' ')) {
            lineStart = idx + 1;
            continue;
        }
        if (chr != '\n') { continue; }
        lineCount ++;

        int lineLength = idx - lineStart;
        if (lineLength == 0) {
            lineStart = idx + 1;
            continue;
        }
        char *line = malloc(lineLength + 1);
    if (!line) {
            kprintf("Error on line: ");
            printU64(lineCount);
            kprintf("\n");
            return NO_MEMORY_ERROR;
        }
        memcpy(line, assembly + lineStart, lineLength);
        line[lineLength] = '\0';

        if (line[lineLength - 1] == ':') {
            char label[64];
            if (makeLabelFromLine(line, label)) {
                free(line);
                kprintf("Error on line: ");
                printU64(lineCount);
                kprintf("\n");
                return LABEL_ERROR;
            }
            if (addNewLabel(state->labels, &state->label_count, label, state->dataOffset)) {
                free(line);
                kprintf("Error on line: ");
                printU64(lineCount);
                kprintf("\n");
                return LABEL_ERROR;
            }
        } else if (memcmp(line, ".string",7) == 0) {
            char *parsed_string = malloc(lineLength - 7); // be generous, don't want overflow after all
            if (getString(line, parsed_string)) {
                free(parsed_string);
                free(line);
                kprintf("Error on line: ");
                printU64(lineCount);
                kprintf("\n");
                return BAD_STRING_ERROR;
            }
            Label *lastLabel = &state->labels[state->labelCount-1];
            if (addNewDataEntry(lastLabel->dataEntries, &lastLabel->entryCount, parsed_string, state->dataOffset)) {
                free(parsed_string);
                free(line);
                kprintf("Error on line: ");
                printU64(lineCount);
                kprintf("\n");
                return BAD_STRING_ERROR;
            }
            state->dataOffset += strlen(parsed_string) + 1;
            free(parsed_string);
        } else {
            if (line[0] == '.') {
                free(line);
                lineStart = idx + 1;
                continue;
            }
            uint8_t instructionLen = getInstructionSize(line);
            if (instructionLen == 0) {
                free(line);
                kprintf("Error on line: ");
                printU64(lineCount);
                kprintf("\n");
                return INSTRUCTION_ERROR;
            }
            state->dataOffset += instructionLen;
        }
        free(line);
        lineStart = idx + 1;
    }
    return 0;
}

void emitImm8(uint8_t* buffer, uint8_t opcode, uint8_t immediate) {
    buffer[0] = opcode;                       
    buffer[1] = immediate; 
}

void emitImm32(uint8_t* buffer, uint8_t opcode, uint64_t immediate) {
    buffer[0] = opcode;                         
    buffer[1] = immediate & 0xFF;
    buffer[2] = (immediate >> 8) & 0xFF;
    buffer[3] = (immediate >> 16) & 0xFF;
    buffer[4] = (immediate >> 24) & 0xFF;
}

typedef struct {
    char mnemonic[32];
    char arg1[32];
    char arg2[32];
} LineInfo;

int getLineInfo(const char* line, LineInfo* info) {
    char instruction[32];
    char operands[2][32];
    uint8_t step = 0; // 0: instruction | 1: operand 1 | 2: operand 2
    uint8_t partIdx = 0;
    int lineLen = (int)strlen(line);
    for (int i = 0; i < lineLen; i++) {
        if (line[i] == ' ' || line[i] == '\t') {
            if (partIdx == 0) continue; // multiple whitespace in a row

            if (step == 0) instruction[partIdx] = '\0';
            else if (step == 1) {
                if (partIdx > 0 && operands[0][partIdx-1] == ',') {
                    operands[0][partIdx-1] = '\0';
                } else {
                    if (i + 1 >= lineLen) return 1;
                    operands[0][partIdx] = '\0';
                }
            }
            else if (step == 2) operands[1][partIdx] = '\0';

            step++;
            if (step == 3) break;
            partIdx = 0;
            continue;
        }

        if (step == 0) instruction[partIdx] = line[i];
        else if (step == 1) operands[0][partIdx] = line[i];
        else if (step == 2) operands[1][partIdx] = line[i];

        partIdx++;
    }
    if (step >= 0 && step < 2 && partIdx == 0) operands[0][0] = '\0';
    if (step >= 0 && step < 3 && partIdx == 0) operands[1][0] = '\0';

    if (step == 0) instruction[partIdx] = '\0';
    else if (step == 1) operands[0][partIdx] = '\0';
    else if (step == 2) operands[1][partIdx] = '\0';

    memcpy(info->mnemonic, instruction, 32);
    memcpy(info->arg1, operands[0], 32);
    memcpy(info->arg2, operands[1], 32);

    return 0;
}

int emitLine(LineInfo* line, uint8_t* byteData, int dataSize, Label labels[256], uint8_t *label_count) {
    uint64_t immediate = 0;
    bool isImmediate = false;
    uint32_t labelValue;
        kprintf("immstr: ");
        kprintf(line->arg1+1);
        kprintf("\n");
    if (!getLabelAddress(labels,label_count,line->arg1+1,&labelValue)) {
        immediate = labelValue;
        isImmediate = true;
    }
    else if (line->arg1[0] == '$') { 
        isImmediate = true;
        if (strlen(line->arg1) >= 3 && line->arg1[2] == 'x') { immediate = hexStringAsU64(line->arg1+1); }
        else if (strlen(line->arg1) > 0) { immediate = stringAsU64(line->arg1+1); }
    }
    if (strcmp(line->mnemonic, "movq") == 0) {
        if (isImmediate) {
            // 1. Calculate ModR/M destination index
            uint8_t modrm = 0x00;
            if      (strcmp(line->arg2, "%rax") == 0) modrm = 0xC0; // 192
            else if (strcmp(line->arg2, "%rcx") == 0) modrm = 0xC1; // 193
            else if (strcmp(line->arg2, "%rdx") == 0) modrm = 0xC2; // 194
            else if (strcmp(line->arg2, "%rbx") == 0) modrm = 0xC3; // 195
            else if (strcmp(line->arg2, "%rdi") == 0) modrm = 0xC7; // 199
            else if (strcmp(line->arg2, "%rsi") == 0) modrm = 0xC6; // 198

            // 2. Write all 7 bytes sequentially without skipping slots
            byteData[0] = 0x48; // REX Prefix (72)
            byteData[1] = 0xC7; // Opcode (199)
            byteData[2] = modrm; // ModR/M Byte (e.g., 192)
            
            // 3. Write 4 bytes of immediate data (Little-Endian)
            byteData[3] = immediate & 0xFF;
            byteData[4] = (immediate >> 8) & 0xFF;
            byteData[5] = (immediate >> 16) & 0xFF;
            byteData[6] = (immediate >> 24) & 0xFF;
        }
    }
    else if (strcmp(line->mnemonic, "int") == 0) {
        if (!isImmediate) return 1;
        emitImm8(byteData, 0xCD, immediate);
    } else {
        kprintf("I don't know what that is ");
        kprintf(line->mnemonic);
        kprintf("|\n");
        return 1;
    }
    return 0;
}

int emitCode(const char* assembly, int size, Label labels[256], uint8_t *label_count, uint8_t* byteData, int dataSize, int textStart) {
    int lineStart = 0;
    int lineCount = 0;
    int dataOffset = 0;
    int commentAdjustment = 0;
    char chr;
    for (int idx = 0; idx < size; idx++) {
        chr = assembly[idx];
        if (lineStart == idx && (chr == '\t' || chr == ' ')) {
            lineStart = idx + 1;
            continue;
        }
        if (chr == '#') { 
            while (chr != '\n') {
                idx++; 
                chr = assembly[idx];
                commentAdjustment ++;
            }
        }
        if (chr != '\n') { continue; }
        lineCount ++;

        int lineLength = idx - lineStart - commentAdjustment;
        commentAdjustment = 0;
        if (lineLength == 0) {
            lineStart = idx + 1;
            continue;
        }
        char *line = malloc(lineLength + 1);
        if (!line) {
            kprintf("Error on line: ");
            printU64(lineCount);
            kprintf("\n");
            return NO_MEMORY_ERROR;
        }
        memcpy(line, assembly + lineStart, lineLength - commentAdjustment);
        line[lineLength] = '\0';

        if (line[lineLength - 1] == ':' || line[0] == '.') {
            free(line);
            lineStart = idx + 1;
            continue;
        }

        LineInfo info;
        if (getLineInfo(line,&info)) {
            free(line);
            kprintf("Lineinfo Error on line: ");
            printU64(lineCount);
            kprintf("\n");
            return INSTRUCTION_ERROR;
        }

        if (emitLine(&info, byteData + dataOffset, dataSize, labels, label_count)) {
            free(line);
            kprintf("emitLine Error on line: ");
            printU64(lineCount);
            kprintf("\n");
            return INSTRUCTION_ERROR;
        }

        uint8_t instructionLen = getInstructionSize(line);
        if (instructionLen == 0) {
            free(line);
            kprintf("Error on line: ");
            printU64(lineCount);
            kprintf("\n");
            return INSTRUCTION_ERROR;
        }
        dataOffset += instructionLen;


        free(line);
        lineStart = idx + 1;
    }
    return 0;
}

void execute_user_program(uint8_t* bytes) {
    void (*func)() = (void (*)())bytes;

    // Set a restoration snapshot point
    if (setjmp(de_stack_env) == 0) {
        func(); // Launch the assembly byte stream
    } else {
        // This block executes instantly when sys_exit calls longjmp!
        kprintf("Returned safely back to kernel control space.\n");
    }
}

AssembledCode assemble(const char* assembly, int size) {
    Label labels[256];
    uint8_t label_count = 0;
    int dataSizeBuffer = 0;
    int textStartBuffer = 0;
    int errorCode;
    errorCode = findLabels(assembly, size, labels, &label_count, &dataSizeBuffer, &textStartBuffer);
    if (errorCode) {
        kprintf("[SCANNER ERROR]: ");
        printU64(errorCode);
        kprintf("\n");
    }
    uint8_t* bytes = malloc(dataSizeBuffer);
    for (int i = 0; i < dataSizeBuffer; i++) {i[bytes] = 0;}
    errorCode = emitCode(assembly, size, labels, &label_count, bytes, dataSizeBuffer, textStartBuffer);
    if (errorCode) {
        kprintf("[EMITTER ERROR]: ");
        printU64(errorCode);
        kprintf("\n");
    }

    kprintf("emitted code with a size of: ");
    printU64(dataSizeBuffer);
    kprintf("\n");

    for (int i = 0; i < dataSizeBuffer; i++){
        printU64(bytes[i]); 
        kprintf(", ");
    }

    kprintf("bytes at: ");
    printU64((uint64_t)bytes);
    kprintf("\n");
    
    execute_user_program(bytes);
}*/
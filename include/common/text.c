#include "text.h"

size_t terminalRow = 0;
size_t terminalColumn = 0;
bool isInitialized = false;


size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

bool strcmp(const char* str1, const char* str2) 
{
    if (strlen(str1) == 0 && strlen(str2) == 0) return true; 
    if (strlen(str1) != strlen(str2)) return false;
	for (int i = 0; i < strlen(str1) && i < strlen(str2); i ++) {
        if (str1[i] != str2[i]) return false;
    }
    return true;
}

// Returns a list with the strings split and a 0xBAADF00D pointer at the end

char* hexAsString(int number) {

    return "NOT-IMPL";
    char buffer[32]; // Enough space for 32-bit int in hex
    int i = 0;
    unsigned int n;

    // Handle 0 explicitly
    if (number == 0) {
        buffer[i++] = '0';
    } else {
        // Convert to unsigned to handle negatives as two's complement
        n = (unsigned int)number;

        while (n > 0) {
            buffer[i++] = "0123456789ABCDEF"[n % 16];
            n /= 16;
        }
    }

    // Prefix with "0x"
    buffer[i++] = 'x';
    buffer[i++] = '0';

    // Null-terminate
    buffer[i] = '\0';

    // Allocate and reverse
    char* result;// = malloc(i + 1); // +1 for '\0'
    if (!result) return NULL;

    for (int j = 0; j < i; j++) {
        result[j] = buffer[i - j - 1];
    }
    result[i] = '\0';

    return result;
}

char* intAsString(int number) {

    char buffer[32]; // temp buffer to build number in reverse
    int i = 0;

    // Handle 0 explicitly
    if (number == 0) {
        buffer[i++] = '0';
    }

    // Handle negative numbers
    int isNegative = 0;
    if (number < 0) {
        isNegative = 1;
        number = -number;
    }

    // Build number in reverse
    while (number > 0) {
        buffer[i++] = '0' + (number % 10);
        number /= 10;
    }

    if (isNegative) {
        buffer[i++] = '-';
    }

    // Null-terminate
    buffer[i] = '\0';

    // Now reverse the buffer
    char result[64];
    for (int j = 0; j < i; j++) {
        result[j] = buffer[i - j - 1];
    }
    result[i] = '\0';

    return result;
}

// Converts `num` to a hex string in `buffer`
// `buffer` must be large enough to hold the result + null terminator.
// Returns pointer to the buffer.
// Example: num = 0x1A3F -> "1A3F"
/*char* hexAsString(uint32_t num, char* buffer) {
    int numLen = 0;
    uint32_t tempNum = num;
    while (tempNum != 0){
        numLen++;
        tempNum = (uint32_t) tempNum / 16;
    }
    const char hex_chars[] = "0123456789ABCDEF";
    int i = numLen - 2; // leave space for null terminator
    buffer[numLen - 1] = '\0';

    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return buffer;
    }

    // Fill buffer from the end (least significant digit)
    while (num > 0 && i >= 0) {
        buffer[i] = hex_chars[num & 0xF];
        num >>= 4;
        i--;
    }

    // Shift result to the front of buffer
    int start = i + 1;
    int j = 0;
    while (buffer[start]) {
        buffer[j++] = buffer[start++];
    }
    buffer[j] = '\0';

    return buffer;
}*/

int stringAsInt(char* buffer) {
    int num = 0;
    bool isNeg = false;
    for (int i = 0; i < strlen(buffer); i++){
        if (buffer[i] == '-') {
            if (i == 0) isNeg = true;
            else return 0;
        }
        num *= 10;
        num = '0' - buffer[i];
    }
    return num;
}

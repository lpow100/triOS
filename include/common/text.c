#include "text.h"
#include "math.h"
#include "io.h"

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

int strcmp(const char* str1, const char* str2) 
{
    if (strlen(str1) == 0 && strlen(str2) == 0) return 0; 
    if (strlen(str1) != strlen(str2)) return 1;
	for (int i = 0; i < strlen(str1) && i < strlen(str2); i ++) {
        if (str1[i] != str2[i]) return 1;
    }
    return 0;
}

int memcmp(const void* str1, const void* str2, size_t n) {
    const unsigned char* s1 = (const unsigned char*)str1;
    const unsigned char* s2 = (const unsigned char*)str2;

    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) {
            return (s1[i] - s2[i]);
        }
    }
    return 0;
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

void intAsString(int number, char* buffer) {
    int i = 0;
    int isNegative = 0;

    // Handle 0 explicitly
    if (number == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    // Handle negative numbers
    if (number < 0) {
        isNegative = 1;
        // Using unsigned to avoid overflow with INT_MIN
        unsigned int n = -number; 
        while (n != 0) {
            buffer[i++] = (n % 10) + '0';
            n /= 10;
        }
    } else {
        unsigned int n = number;
        while (n != 0) {
            buffer[i++] = (n % 10) + '0';
            n /= 10;
        }
    }

    if (isNegative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0'; // Always null-terminate!

    // The digits are currently backwards (e.g., "321" for 123)
    // We need to reverse the string in place
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

void u64AsString(uint64_t number, char* buffer) {
    int i = 0;

    // Handle 0 case
    if (number == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    // Extract digits (they come out backwards)
    while (number > 0) {
        buffer[i++] = (number % 10) + '0';
        number /= 10;
    }

    buffer[i] = '\0'; // Null terminate

    // Reverse the string in place
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
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

void printU64(int number) {
    char buffer[21];
    int i = 0;

    // Handle 0 case
    if (number == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        kprintf(buffer);
        return;
    }

    while (number > 0) {
        buffer[i++] = (number % 10) + '0';
        number /= 10;
    }

    buffer[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }

    kprintf(buffer);
}

uint64_t stringAsU64(const char* buffer) {
    uint64_t result = 0;
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] < '0' || buffer[i] > '9') break;
        result = result * 10 + (buffer[i] - '0');
    }
    return result;
}

uint64_t hexStringAsU64(const char* buffer) {
    uint64_t result = 0;
    int i = 0;
    if (buffer[0] == '0' && buffer[1] == 'x') i = 2; // skip 0x prefix
    for (; buffer[i] != '\0'; i++) {
        uint8_t digit;
        if (buffer[i] >= '0' && buffer[i] <= '9') digit = buffer[i] - '0';
        else if (buffer[i] >= 'a' && buffer[i] <= 'f') digit = buffer[i] - 'a' + 10;
        else if (buffer[i] >= 'A' && buffer[i] <= 'F') digit = buffer[i] - 'A' + 10;
        else break;
        result = result * 16 + digit;
    }
    return result;
}
/*
 * Creator: Naman Dixit
 * Notice: © Copyright 2019 Naman Dixit
 */

#if !defined(UNICODE_H_INCLUDE_GUARD)

header_function
B64 unicodeCodepointFromUTF16Surrogate (U16 surrogate, U16 *last_surrogate, U32 *codepoint)
{
    U16 utf16_hi_surrogate_start = 0xD800;
    U16 utf16_lo_surrogate_start = 0xDC00;
    U16 utf16_surrogate_end = 0xDFFF;

    if ((surrogate >= utf16_hi_surrogate_start) &&
        (surrogate < utf16_lo_surrogate_start)) {
        *last_surrogate = surrogate;

        return false;
    } else {
        if ((surrogate >= utf16_lo_surrogate_start) &&
            (surrogate <= utf16_surrogate_end)) {
            U16 low_surrogate = surrogate;
            // NOTE(naman): In this line, the numbers get promoted from U16 to S32,
            // so storing them in a U32 results in a inmpicit sign conversion.
            // That is why we are casting manually.
            *codepoint = (U32)((*last_surrogate - utf16_hi_surrogate_start) << 10U);
            *codepoint |= (low_surrogate - utf16_lo_surrogate_start);
            *codepoint += 0x10000;

            *last_surrogate = 0;
        } else {
            *codepoint = surrogate;
        }

        return true;
    }
}

header_function
Size unicodeUTF8FromUTF32 (U32 *codepoints, Size codepoint_count, Char *buffer)
{
    if (buffer == NULL) {
        Size length = 1; // NOTE(naman): We need one byte for the NUL byte.

        for (Size i = 0; i < codepoint_count; i++) {
            if (codepoints[i] <= 0x7F) {
                length += 1;
            } else if (codepoints[i] <= 0x7FF) {
                length += 2;
            } else if (codepoints[i] <= 0xFFFF) {
                length += 3;
            } else if (codepoints[i] <= 0x10FFFF) {
                length += 4;
            }
        }

        return length;
    } else {
        Size length = 1; // NOTE(naman): We need one byte for the NUL byte.

        for (Size i = 0; i < codepoint_count; i++) {
            if (codepoints[i] <= 0x7F) {
                buffer[0] = (Char)codepoints[i];
                buffer += 1;
                length += 1;
            } else if (codepoints[i] <= 0x7FF) {
                buffer[0] = (Char)(0xC0 | (codepoints[i] >> 6));            /* 110xxxxx */
                buffer[1] = (Char)(0x80 | (codepoints[i] & 0x3F));          /* 10xxxxxx */
                buffer += 2;
                length += 2;
            } else if (codepoints[i] <= 0xFFFF) {
                buffer[0] = (Char)(0xE0 | (codepoints[i] >> 12));           /* 1110xxxx */
                buffer[1] = (Char)(0x80 | ((codepoints[i] >> 6) & 0x3F));   /* 10xxxxxx */
                buffer[2] = (Char)(0x80 | (codepoints[i] & 0x3F));          /* 10xxxxxx */
                buffer += 3;
                length += 3;
            } else if (codepoints[i] <= 0x10FFFF) {
                buffer[0] = (Char)(0xF0 | (codepoints[i] >> 18));           /* 11110xxx */
                buffer[1] = (Char)(0x80 | ((codepoints[i] >> 12) & 0x3F));  /* 10xxxxxx */
                buffer[2] = (Char)(0x80 | ((codepoints[i] >> 6) & 0x3F));   /* 10xxxxxx */
                buffer[3] = (Char)(0x80 | (codepoints[i] & 0x3F));          /* 10xxxxxx */
                buffer += 4;
                length += 4;
            }
        }

        buffer[0] = '\0';

        return length;
    }
}

# if defined(OS_WINDOWS)

header_function
LPWSTR unicodeWin32UTF16FromUTF8 (Char *utf8)
{
    int wcstr_length = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    LPWSTR wcstr = VirtualAlloc(NULL, (DWORD)wcstr_length * sizeof(wchar_t),
                                MEM_COMMIT, PAGE_READWRITE);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wcstr, wcstr_length);

    int normalized_length = NormalizeString(NormalizationC,
                                            wcstr, -1,
                                            NULL, 0);
    LPWSTR norm = VirtualAlloc(NULL, (DWORD)normalized_length * sizeof(wchar_t),
                               MEM_COMMIT, PAGE_READWRITE);
    NormalizeString(NormalizationC, wcstr, -1, norm, normalized_length);

    VirtualFree(wcstr, 0, MEM_RELEASE);

    return norm;
}

header_function
void unicodeWin32UTF16Dealloc (LPWSTR utf16)
{
    VirtualFree(utf16, 0, MEM_RELEASE);
}

# endif

#define UNICODE_H_INCLUDE_GUARD
#endif

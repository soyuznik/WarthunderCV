#pragma once
#include <windows.h>
#include <iostream>

std::wstring get_utf16(const std::string& str, int codepage = GetACP())
{
    if (str.empty()) return std::wstring();
    int sz = MultiByteToWideChar(codepage, 0, &str[0], (int)str.size(), 0, 0);
    std::wstring res(sz, 0);
    MultiByteToWideChar(codepage, 0, &str[0], (int)str.size(), &res[0], sz);
    return res;
}
LPCWSTR str_to_lpcwstr1(std::string s) {
    std::wstring stemp = get_utf16(s);
	LPCWSTR sw = stemp.c_str();
	return sw;
}

LPCWSTR str_to_lpcwstr(std::string s) {
    std::wostringstream woss;
    std::wstring wstr(s.begin(), s.end());
    woss << wstr;
    return woss.str().c_str();
}
///std::wstring strg(const std::string& text) {
//	return std::wstring(text.begin(), text.end());
//}
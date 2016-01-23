#pragma once

#include <Windows.h>
#include <stdio.h>
#include <vector>
#include "fileop.h"
#include <io.h>
#include <fcntl.h>
#include <clocale>
#include "..\shared\message.h"
#include "..\shared\translation.h"

struct gxt_tag {
    unsigned short symbol;
    std::wstring tagname;
};

struct gxt_tag_repair_helper {
    std::wstring what;
    std::wstring to;

    gxt_tag_repair_helper(wchar_t *What, wchar_t *To) {
        what.append(What);
        to.append(To);
    }
};

std::vector<std::pair<unsigned short, unsigned short>> mapping;
std::vector<gxt_tag> tags;
std::vector<gxt_tag_repair_helper> repairTags;
bool useMapping;
bool addTranslationLine;
bool showWarnMessage;
bool addTranslatedText;

void readSettingsFiles() {
    wchar_t line[256]; wchar_t s_old[16]; wchar_t s_new[16];
    useMapping = true;
    addTranslationLine = false;
    showWarnMessage = true;

    setlocale(LC_ALL, "");
    wprintf(L"Reading settings ...\n");
    FILE *file;
    _wfopen_s(&file, L"settings.dat", L"r");
    if (file) {
        _setmode(_fileno(file), _O_WTEXT);
        skipBom(file);
        wchar_t param_name[64]; unsigned int param;
        if (readline(line, 256, file) && swscanf(line, L"%s %d", param_name, &param) == 2 && !wcscmp(param_name, L"SHOW_WARN_MESSAGE")) {
            showWarnMessage = param == 1;
            wprintf(L"SHOW_WARN_MESSAGE      %d\n", showWarnMessage);
        }
        else
            wprintf(L"SHOW_WARN_MESSAGE      UNKNOWN\n");
        if (readline(line, 256, file) && swscanf(line, L"%s %d", param_name, &param) == 2 && !wcscmp(param_name, L"USE_MAPPING")) {
            useMapping = param == 1;
            wprintf(L"USE_MAPPING            %d\n", useMapping);
        }
        else
            wprintf(L"USE_MAPPING            UNKNOWN\n");
        if (readline(line, 256, file) && swscanf(line, L"%s %d", param_name, &param) == 2 && !wcscmp(param_name, L"ADD_TRANSLATION_LINE")) {
            addTranslationLine = param == 1;
            wprintf(L"ADD_TRANSLATION_LINE   %d\n", addTranslationLine);
        }
        else
            wprintf(L"ADD_TRANSLATION_LINE   UNKNOWN\n");
        if (readline(line, 256, file) && swscanf(line, L"%s %d", param_name, &param) == 2 && !wcscmp(param_name, L"ADD_TRANSLATED_TEXT")) {
            addTranslatedText = param == 1;
            wprintf(L"ADD_TRANSLATED_TEXT    %d\n", addTranslatedText);
        }
        else
            wprintf(L"ADD_TRANSLATED_TEXT    UNKNOWN\n");
        if (readline(line, 256, file) && swscanf(line, L"%s %s", param_name, translation_api::translatingLang) == 2 && !wcscmp(param_name, L"TRANSLATION_LANGUAGE")) {
            wprintf(L"TRANSLATION_LANGUAGE   %s\n", translation_api::translatingLang);
        }
        else {
            wprintf(L"TRANSLATION_LANGUAGE   UNKNOWN\n");
            translation_api::translatingApiKey[0] = 0;
        }
        if (readline(line, 256, file) && swscanf(line, L"%s %s", param_name, translation_api::translatingApiKey) == 2 && !wcscmp(param_name, L"ONLINE_TRANSLATING_KEY")) {
            wprintf(L"ONLINE_TRANSLATING_KEY %s\n", translation_api::translatingApiKey);
        }
        else {
            wprintf(L"ONLINE_TRANSLATING_KEY UNKNOWN\n");
            translation_api::translatingApiKey[0] = 0;
        }
        fclose(file);
        wprintf(L"Done\n");
    }
    else
        wprintf(L"File (\"settings.dat\") not found\n");

    wprintf(L"Reading mapping ...\n");
    _wfopen_s(&file, L"mapping.dat", L"r");
    if (file) {
        _setmode(_fileno(file), _O_WTEXT);
        skipBom(file);
        while (readline(line, 256, file)) {
            if (swscanf(line, L"%s %s", s_old, s_new) == 2)
                mapping.push_back(std::make_pair(symbol_to_value(s_old), symbol_to_value(s_new)));
        }
        fclose(file);
        unsigned int n = mapping.size() / 3;
        unsigned int m = mapping.size() % 3;
        for (unsigned int i = 0; i < n; i++) {
            for (unsigned int j = 0; j < 3; j++) {
                wprintf(L"[0x%X '%lc' > 0x%X '%lc']", mapping[j + i * 3].first, mapping[j + i * 3].first, mapping[j + i * 3].second, mapping[j + i * 3].second);
                if (j == 2)
                    wprintf(L"\n");
                else
                    wprintf(L" ");
            }
        }
        for (unsigned int i = 0; i < m; i++) {
            wprintf(L"[0x%X '%lc' > 0x%X '%lc']", mapping[i + n * 3].first, mapping[i + n * 3].first, mapping[i + n * 3].second, mapping[i + n * 3].second);
            if (i != (m - 1))
                wprintf(L" ");
            else
                wprintf(L"\n");
        }
        wprintf(L"Done\n");
    }
    else
        wprintf(L"File (\"mapping.dat\") not found\n");

    wprintf(L"Reading tags ...\n");
    _wfopen_s(&file, L"tags.dat", L"r");
    if (file) {
        _setmode(_fileno(file), _O_WTEXT);
        skipBom(file);
        wchar_t tag_symbol[16]; wchar_t tag_str[128];
        while (readline(line, 256, file)) {
            if (swscanf(line, L"%s %s", tag_symbol, tag_str) == 2) {
                tags.resize(tags.size() + 1);
                tags[tags.size() - 1].symbol = symbol_to_value(tag_symbol);
                tags[tags.size() - 1].tagname.assign(tag_str);
            }
        }
        fclose(file);
        unsigned int n = tags.size() / 3;
        unsigned int m = tags.size() % 3;
        for (unsigned int i = 0; i < n; i++) {
            for (unsigned int j = 0; j < 3; j++) {
                wprintf(L"[0x%X > ~%s~]", tags[j + i * 3].symbol, tags[j + i * 3].tagname.c_str());
                if (j == 2)
                    wprintf(L"\n");
                else
                    wprintf(L" ");
            }
        }
        for (unsigned int i = 0; i < m; i++) {
            wprintf(L"[0x%X > ~%s~]", tags[i + n * 3].symbol, tags[i + n * 3].tagname.c_str());
            if (i != (m - 1))
                wprintf(L" ");
            else
                wprintf(L"\n");
        }
        wprintf(L"Done\n\n");
    }
    else
        wprintf(L"File (\"tags.dat\") not found\n");
    setlocale(LC_ALL, "C");
}

wchar_t *getTag(unsigned short symbol) {
    for (unsigned int i = 0; i < tags.size(); i++) {
        if (tags[i].symbol == symbol)
            return (wchar_t *)tags[i].tagname.c_str();
    }
    return NULL;
}

unsigned int tagToSymbol(unsigned int lineNumber, std::wstring& input, unsigned int inputPosn, std::wstring& output) {
    std::wstring tag;
    bool tagClosed = false;
    for (unsigned int i = inputPosn + 1; i < input.size(); i++) {
        if (input[i] == '~') {
            tagClosed = true;
            break;
        }
        tag.push_back(input[i]);
    }
    if (tagClosed) {
        if (tag.size() == 1 && tag[0] == 'n') {
            output.push_back('\n');
            return 1;
        }
        else if (tag.size() == 5 && tag[0] == '#') {
            unsigned int result = 0;
            if (swscanf(&tag.c_str()[1], L"%X", &result) == 1) {
                output.push_back(result);
                return 5;
            }
        }
        else {
            for (unsigned int i = 0; i < tags.size(); i++) {
                if (!_wcsicmp(tags[i].tagname.c_str(), tag.c_str())) {
                    output.push_back(tags[i].symbol);
                    return tags[i].tagname.size();
                }
            }
        }
    }
    else {
        unsigned int tagsCountBeforeThisTag = 0;
        for (unsigned int i = 0; i < inputPosn; i++) {
            if (input[i] == '~')
                tagsCountBeforeThisTag++;
        }
        if (!(tagsCountBeforeThisTag % 2)) {
            if (tag.size() > 16) {
                wprintf(L"\n   warning (line %d): possibly a wrong tag ('~%.16s...')", lineNumber, tag.c_str());
                if (showWarnMessage)
                    show_message(L"warning (line %d): possibly a wrong tag ('~%.16s...')", lineNumber, tag.c_str());
            }
            else {
                wprintf(L"\n   warning (line %d): possibly a wrong tag ('~%s')", lineNumber, tag.c_str());
                if (showWarnMessage)
                    show_message(L"warning (line %d): possibly a wrong tag ('~%s')", lineNumber, tag.c_str());
            }
        }
        return 0;
    }
    if (tag.size() > 32) {
        wprintf(L"\n   warning (line %d): tag '~%.32s...' is not defined", lineNumber, tag.c_str());
        if (showWarnMessage)
            show_message(L"warning (line %d): tag '~%.32s...' is not defined", lineNumber, tag.c_str());
    }
    else {
        wprintf(L"\n   warning (line %d): tag '~%s~' is not defined", lineNumber, tag.c_str());
        if (showWarnMessage)
            show_message(L"warning (line %d): tag '~%s~' is not defined", lineNumber, tag.c_str());
    }
    return 0;
}

std::wstring replaceTextTagsForTranslation(std::wstring &str) {
    repairTags.clear();
    std::wstring output;
    for (unsigned int s = 0; s < str.size(); s++) {
        if (str[s] == '~') {
            std::wstring tag;
            bool tagClosed = false;
            for (unsigned int i = s + 1; i < str.size(); i++) {
                if (str[i] == '~') {
                    tagClosed = true;
                    break;
                }
                tag.push_back(str[i]);
            }
            if (tagClosed) {
                if (tag.size() == 1 && tag[0] == 'n') {
                    output.append(L"(TAG_T_n)");
                    s += 2;
                    repairTags.push_back(gxt_tag_repair_helper(L"(TAG_T_n)", L"~n~"));
                }
                else if (tag.size() == 5 && tag[0] == '#') {
                    unsigned int result = 0;
                    if (swscanf(&tag.c_str()[1], L"%X", &result) == 1) {
                        std::wstring newtag, oldtag;
                        newtag.append(L"(TAG_N_");
                        newtag.append(&tag.c_str()[1]);
                        newtag.append(L")");
                        output.append(newtag);
                        oldtag.append(L"~");
                        oldtag.append(tag);
                        oldtag.append(L"~");
                        s += 6;
                        repairTags.push_back(gxt_tag_repair_helper((wchar_t *)newtag.c_str(), (wchar_t *)oldtag.c_str()));
                    }
                }
                else {
                    for (unsigned int i = 0; i < tags.size(); i++) {
                        if (!_wcsicmp(tags[i].tagname.c_str(), tag.c_str())) {
                            std::wstring newtag, oldtag;
                            newtag.append(L"(TAG_T_");
                            newtag.append(tag);
                            newtag.append(L")");
                            output.append(newtag);
                            oldtag.append(L"~");
                            oldtag.append(tag);
                            oldtag.append(L"~");
                            s += tag.size() + 1;
                            repairTags.push_back(gxt_tag_repair_helper((wchar_t *)newtag.c_str(), (wchar_t *)oldtag.c_str()));
                        }
                    }
                }
            }
            else
                output.push_back(str[s]);
        }
        else
            output.push_back(str[s]);
    }
    return output;
}

void repairTextTagsAfterTranslation(std::wstring &str) {
    std::wstring output;
    for (unsigned int i = 0; i < repairTags.size(); i++)
        replaceAll(str, repairTags[i].what, repairTags[i].to);
}

unsigned short getMappedSymbol(unsigned short symbol) {
    if (useMapping) {
        for (unsigned int i = 0; i < mapping.size(); i++) {
            if (mapping[i].first == symbol)
                return mapping[i].second;
        }
    }
    return symbol;
}

unsigned short unmapSymbol(unsigned short symbol) {
    if (useMapping) {
        for (unsigned int i = 0; i < mapping.size(); i++) {
            if (mapping[i].second == symbol)
                return mapping[i].first;
        }
    }
    return symbol;
}

struct gxt_file {
    char signature[6]; // "DS_GXT"
    unsigned short numStrings;
    // strings...
    // unsigned short numSymbols;
    // wchar_t symbols[numSymbols]; // numSymbols+1 for last string (this includes terminating null)
    // ...
};

class cGXTFile {
public:
    bool loaded;
    bool hasWarnings;
    std::vector<std::wstring> strings;

    cGXTFile(wchar_t *filepath) {
        read(filepath);
    }

    cGXTFile() {
        loaded = false;
        hasWarnings = false;
    }

    bool read(wchar_t *filepath) {
        strings.clear();
        loaded = false;
        hasWarnings = false;
        FILE *file = _wfopen(filepath, L"rb");
        if (file) {
            fseek(file, 0, SEEK_END);
            unsigned int size = ftell(file);
            if (size > 7) {
                fseek(file, 0, SEEK_SET);
                gxt_file header;
                fread(&header, 8, 1, file);
                if (!strncmp(header.signature, "DS_GXT", 6)) {
                    if (header.numStrings > 0) {
                        strings.resize(header.numStrings);
                        for (unsigned int i = 0; i < header.numStrings; i++) {
                            unsigned short strLength;
                            fread(&strLength, 2, 1, file);
                            wchar_t *string = new wchar_t[strLength];
                            fread(string, strLength * 2, 1, file);
                            for (unsigned int c = 0; c < strLength; c++)
                                strings[i].push_back(string[c]);
                            delete[] string;
                        }
                    }
                    loaded = true;
                }
                else {
                    if (showWarnMessage)
                        show_message(L"file \"%s\" is not a valid gxt file", filepath);
                }
            }
            else {
                if (showWarnMessage)
                    show_message(L"file \"%s\" is not a valid gxt file", filepath);
            }
            fclose(file);
        }
        else {
            if (showWarnMessage)
                show_message(L"failed to open file \"%s\"", filepath);
        }
        if (!loaded && strings.size() > 0)
            strings.clear();
        return loaded;
    }

    bool readFromTextFile(wchar_t *filepath) {
        strings.clear();
        loaded = false;
        hasWarnings = false;
        FILE *file = NULL;
        file = _wfopen(filepath, L"r");
        _setmode(_fileno(file), _O_WTEXT);
        skipBom(file);
        std::wstring line;
        if (read_wholeline(line, file) && !wcsncmp(line.c_str(), L"GXT", 3)) {
            while (read_wholeline(line, file)) {
                strings.resize(strings.size() + 1);
                for (unsigned int c = 0; c < line.size(); c++) {
                    if (line[c] == '~') {
                        unsigned int tagSize = tagToSymbol(strings.size() + 1, line, c, strings[strings.size() - 1]);
                        if (tagSize != 0)
                            c += tagSize + 1;
                        else {
                            strings[strings.size() - 1].push_back(unmapSymbol(line[c]));
                            hasWarnings = true;
                        }
                    }
                    else
                        strings[strings.size() - 1].push_back(unmapSymbol(line[c]));
                }
                if (strings[strings.size() - 1].size() == 0) {
                    wprintf(L"\n   warning (line %d): line is empty", strings.size() + 1);
                    if (showWarnMessage)
                        show_message(L"warning (line %d): line is empty", strings.size() + 1);
                    hasWarnings = true;
                }
            }
            loaded = true;
        }
        else {
            if (showWarnMessage)
                show_message(L"file \"%s\" is not a valid gxt file", filepath);
        }
        fclose(file);
        if (!loaded && strings.size() > 0)
            strings.clear();
        return loaded;
    }

    void write(wchar_t *outputFilePath) {
        FILE *file = _wfopen(outputFilePath, L"wb");
        fwrite("DS_GXT", 6, 1, file);
        writevalue(file, strings.size());
        for (unsigned int i = 0; i < strings.size(); i++) {
            if(i == strings.size() - 1)
                writevalue(file, strings[i].size() + 1);
            else
                writevalue(file, strings[i].size());
            for (unsigned int j = 0; j < strings[i].size(); j++)
                writevalue(file, strings[i][j]);
        }
        writevalue(file, 0);
        fclose(file);
    }

    void writeToTexFile(wchar_t *outputFilePath) {
        FILE *file = _wfopen(outputFilePath, L"wb");
        fwrite("\xFF\xFE", 2, 1, file);
        fputws(L"GXT", file);
        if (addTranslatedText) {
            fputws(L" | Переведено «Яндекс.Переводчиком»", file);
            if (translation_api::translatingLang[0]) {
                fputws(L" (", file);
                fputws(translation_api::translatingLang, file);
                fputws(L")", file);
            }
        }
        for (unsigned int i = 0; i < strings.size(); i++) {
            if (i == 0)
                fputws(L"\r\n", file);
            std::wstring formatted_str;
            for (unsigned int c = 0; c < strings[i].size(); c++) {
                if (strings[i][c] > 0xFEEF) {
                    if (getTag(strings[i][c])) {
                        formatted_str.push_back('~');
                        formatted_str.append(getTag(strings[i][c]));
                        formatted_str.push_back('~');
                    }
                    else {
                        wchar_t tmp[16];
                        wsprintfW(tmp, L"~#%X~", strings[i][c]);
                        formatted_str.append(tmp);
                    }
                }
                else if (strings[i][c] == 0xA)
                    formatted_str.append(L"~n~");
                else
                    formatted_str.push_back(getMappedSymbol(strings[i][c]));
            }
            if (addTranslationLine)
                fputws(L";;;", file);
            fputws(formatted_str.c_str(), file);
            if (addTranslationLine) {
                fputws(L"\r\n", file);
                if (addTranslatedText) {
                    std::wstring translated = translation_api::translate(replaceTextTagsForTranslation(formatted_str));
                    repairTextTagsAfterTranslation(translated);
                    fputws(translated.c_str(), file);
                }
            }
            if (i != (strings.size() - 1))
                fputws(L"\r\n", file);
        }
        fclose(file);
    }
};
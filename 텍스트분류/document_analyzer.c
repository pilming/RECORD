#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>    
#include "document_analyzer.h"
#define TRUE (1)
#define FALSE (0)


char* g_file_contents = NULL;
size_t g_file_size = 0; /*�ι��� ������ ����ũ�� = ���� ����*/
char**** g_file_tokens = NULL;

int load_document(const char* document)
{
    FILE* stream;
    char**** temp_file_token; /*���ܵ��� �ӽ÷� ��� ��ū*/
    char*** temp_snt_token; /*������� �ӽ÷� ��� ��ū */
    char** temp_wrd_token; /*�ܾ���� �ӽ÷� ��� ��ū*/
    char* temp_char_token; /*���ڵ��� �ӽ÷� ��� ��ū*/
    char delims[7] = { ' ', ',', '.', '?', '!', '\n', 0 };
    size_t char_count = 0;
    size_t prg_index = 0;
    size_t snt_index = 0;
    size_t wrd_index = 0;
    size_t new_line_count = 0;
    size_t i = 0;
    size_t j = 0;
    size_t prg_size;
    size_t snt_size;
    size_t wrd_size;
    size_t char_size;

    stream = fopen(document, "r");

    if (stream == NULL) {
        return FALSE;
    }

    fseek(stream, 0, SEEK_END); /*�����͸� ���� ������ �̵�*/

    g_file_size = ftell(stream); /*����ũ�� �ľ�*/

    g_file_contents = malloc(g_file_size + 1); /*�������Ͽ� �Ҵ�.*/

    if (g_file_contents == NULL) { /*���� �����������Ͱ����� vs���� ��� �ߴ°� �Ⱦ ���� �ڵ�*/
        return FALSE;
    }

    memset(g_file_contents, 0, g_file_size + 1);

    fseek(stream, 0, SEEK_SET); /*�����͸� ���� �������� �̵�*/

    fread(g_file_contents, g_file_size + 1, 1, stream);

    fclose(stream);

    if (g_file_size == 0) {
        g_file_tokens = NULL;
        return TRUE;
    }

    prg_size = sizeof(char***) * 2;
    snt_size = sizeof(char**) * 2;
    wrd_size = sizeof(char*) * 2;
    char_size = sizeof(char);

    g_file_tokens = (char****)malloc(prg_size); /*���� ũ��*/
    if (!g_file_tokens) {
        return FALSE;
    }

    g_file_tokens[prg_index] = (char***)malloc(snt_size); /* ���� 1�� ũ��*/
    if (!g_file_tokens[prg_index]) {
        free(g_file_tokens);
        return FALSE;
    }
    g_file_tokens[prg_index][snt_index] = (char**)malloc(wrd_size); /*�ܾ� 1�� ũ��*/
    if (!g_file_tokens[prg_index][snt_index]) {
        free(g_file_tokens[prg_index]);
        return FALSE;
    }
    g_file_tokens[prg_index][snt_index][wrd_index] = (char*)malloc(char_size); /*���� 1�� ũ��*/
    if (!g_file_tokens[prg_index][snt_index][wrd_index]) {
        free(g_file_tokens[prg_index][snt_index][wrd_index]);
        return FALSE;
    }

    for (i = 0; i < g_file_size; i++) {
        for (j = 0; j < strlen(delims); j++) {
            if (g_file_contents[i] == delims[j]) {
                if (delims[j] == ' ' || delims[j] == ',') {
                    if (char_count != 0) {
                        wrd_index++;
                        wrd_size = wrd_size + sizeof(char*);
                        temp_wrd_token = (char**)malloc(wrd_size); /*�ܾ� 1�� ũ�⸸ŭ �߰��� �Ҵ���� 1���� �ι��� �ڸ�*/
                        if (temp_wrd_token != NULL) {
                            memcpy(temp_wrd_token, g_file_tokens[prg_index][snt_index], wrd_size - sizeof(char*));
                            free(g_file_tokens[prg_index][snt_index]);
                            g_file_tokens[prg_index][snt_index] = temp_wrd_token;
                            g_file_tokens[prg_index][snt_index][wrd_index + 1] = NULL;
                            char_size = sizeof(char);
                            g_file_tokens[prg_index][snt_index][wrd_index] = (char*)malloc(char_size); /*�����ܾ� ����1�� ũ�� �Ҵ� �����̶� �޸� ������ �׻� ���� �ܾ ���⶧���� ������*/
                        }
                        char_count = 0;
                        goto forward;
                    }

                    else {
                        goto forward;
                    }
                }

                else if (delims[j] == '.' || delims[j] == '!' || delims[j] == '?') {
                    g_file_tokens[prg_index][snt_index][wrd_index + 1] = NULL;
                    snt_index++;
                    wrd_index = 0;
                    snt_size = snt_size + sizeof(char**);

                    if (g_file_contents[i + 1] == '\n' || g_file_contents[i + 1] == '\0') {
                        g_file_tokens[prg_index][snt_index] = NULL; /*������ �ٹٲ��̰ų� ���ϳ��̸� ���峡.*/
                        if (g_file_contents[i + 1] == '\0') { /*������ ��*/
                            goto finish;
                        }
                    }


                    else {
                        temp_snt_token = (char***)malloc(snt_size); /*���� 1�� ũ�⸸ŭ �߰��� �Ҵ���� ���ڸ� ������*/
                        if (temp_snt_token != NULL) {
                            memcpy(temp_snt_token, g_file_tokens[prg_index], snt_size - sizeof(char**));
                            free(g_file_tokens[prg_index]);
                            g_file_tokens[prg_index] = temp_snt_token;
                            wrd_size = sizeof(char*) * 2;
                            g_file_tokens[prg_index][snt_index + 1] = NULL;
                            g_file_tokens[prg_index][snt_index] = (char**)malloc(wrd_size); /*�ܾ� 1�� ũ�� �Ҵ�*/
                            char_size = sizeof(char);
                            g_file_tokens[prg_index][snt_index][wrd_index] = (char*)malloc(char_size); /*�����ܾ� ����1�� ũ�� �Ҵ�*/

                        }

                    }

                    char_count = 0;
                    goto forward;
                }

                else if (delims[j] == '\n') {
                    new_line_count++;
                    if (new_line_count == 2) {
                        prg_size = prg_size + sizeof(char***);
                        temp_file_token = (char****)malloc(prg_size); /*���� 1�� ũ�⸸ŭ �߰��� �Ҵ���� ���ڸ� ������*/
                        if (temp_file_token != NULL) {
                            memcpy(temp_file_token, g_file_tokens, prg_size - sizeof(char***));
                            free(g_file_tokens);
                            g_file_tokens = temp_file_token;
                        }
                        new_line_count = 0;
                        prg_index++;
                        snt_index = 0;
                        wrd_index = 0;
                        snt_size = sizeof(char**) * 2;
                        wrd_size = sizeof(char*) * 2;
                        char_size = sizeof(char);
                        g_file_tokens[prg_index] = (char***)malloc(snt_size); /*����2�� ũ�� �Ҵ�*/
                        g_file_tokens[prg_index][snt_index] = (char**)malloc(wrd_size); /*�ܾ� 2�� ũ�� �Ҵ�*/
                        g_file_tokens[prg_index][snt_index][wrd_index] = (char*)malloc(char_size); /*�����ܾ� ����1�� ũ�� �Ҵ�*/
                        goto forward; /*������ �Ѿ������ �׻� ������ �����ϱ� char_count �� 0���� �����ʿ� ����*/
                    }

                    else {
                        goto forward;
                    }

                }
            }
        }
        /*���й��ڰ� �ƴ϶� �Ϲ� �����ϰ��*/

        char_count++;
        char_size = char_size + sizeof(char);
        temp_char_token = (char*)malloc(char_size); /*���� 1�� ũ�⸸ŭ �߰��� �Ҵ���� �ι��� �ڸ�*/
        if (temp_char_token != NULL) {
            memcpy(temp_char_token, g_file_tokens[prg_index][snt_index][wrd_index], char_size - sizeof(char));
            free(g_file_tokens[prg_index][snt_index][wrd_index]);
            g_file_tokens[prg_index][snt_index][wrd_index] = temp_char_token;
            g_file_tokens[prg_index][snt_index][wrd_index][char_count - 1] = g_file_contents[i];
            g_file_tokens[prg_index][snt_index][wrd_index][char_count] = '\0';
        }

    forward:
        continue;
    }

finish:
    g_file_tokens[++prg_index] = NULL; /*������ null*/

    return TRUE;
}

void dispose(void)
{
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;

    if (g_file_tokens != NULL) {
        if (g_file_tokens[i]) {
            while (g_file_tokens[i] != NULL) {
                while (g_file_tokens[i][j] != NULL) {
                    while (g_file_tokens[i][j][k] != NULL) {
                        free(g_file_tokens[i][j][k]);
                        g_file_tokens[i][j][k] = NULL;
                        k++;
                    }
                    free(g_file_tokens[i][j][k]); /*�� �ڸ����� �Ҵ� �޾ұ⶧����*/
                    g_file_tokens[i][j][k] = NULL;
                    k = 0;
                    free(g_file_tokens[i][j]);
                    g_file_tokens[i][j] = NULL;
                    j++;
                }
                free(g_file_tokens[i][j]);
                g_file_tokens[i][j] = NULL;
                j = 0;
                free(g_file_tokens[i]);
                g_file_tokens[i] = NULL;
                i++;
            }
            free(g_file_tokens[i]);
            g_file_tokens[i] = NULL;
            free(g_file_tokens);
            g_file_tokens = NULL;
        }
    }
    free(g_file_contents);
    g_file_contents = NULL;
}

size_t get_total_word_count(void)
{
    size_t word_count = 0;
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;

    if (g_file_contents == NULL || g_file_size == 0) {
        return 0;
    }

    while (g_file_tokens[i] != NULL) {
        while (g_file_tokens[i][j] != NULL) {
            while (g_file_tokens[i][j][k] != NULL) {
                word_count++;
                k++;
            }
            k = 0;
            j++;
        }
        j = 0;
        i++;
    }

    return word_count;
}

size_t get_total_sentence_count(void)
{
    size_t sentence_count = 0;
    size_t i = 0;
    size_t j = 0;

    if (g_file_contents == NULL || g_file_size == 0) {
        return 0;
    }

    while (g_file_tokens[i] != NULL) {
        while (g_file_tokens[i][j] != NULL) {
            sentence_count++;
            j++;
        }
        j = 0;
        i++;
    }

    return sentence_count;

}

size_t get_total_paragraph_count(void)
{
    size_t paragraph_count = 0;
    size_t i = 0;

    if (g_file_contents == NULL || g_file_size == 0) {
        return 0;
    }

    while (g_file_tokens[i] != NULL) {
        paragraph_count++;
        i++;
    }

    return paragraph_count;

}

const char*** get_paragraph(const size_t paragraph_index)
{
    size_t paragraph_count = get_total_paragraph_count();


    if (paragraph_index >= paragraph_count || g_file_contents == NULL) {
        return NULL;
    }


    return (const char***)g_file_tokens[paragraph_index];

}

size_t get_paragraph_word_count(const char*** paragraph)
{
    size_t result = 0;
    size_t i = 0;
    size_t j = 0;

    while (paragraph[i] != NULL) {
        while (paragraph[i][j] != NULL) {
            result++;
            j++;
        }
        j = 0;
        i++;
    }

    return result;
}
size_t get_paragraph_sentence_count(const char*** paragraph)
{
    size_t result = 0;
    size_t i = 0;

    while (paragraph[i] != NULL) {
        result++;
        i++;
    }

    return result;
}

const char** get_sentence(const size_t paragraph_index, const size_t sentence_index)
{
    size_t paragraph_count = get_total_paragraph_count();
    size_t sentence_count;

    if (paragraph_index >= paragraph_count || g_file_contents == NULL) {
        return NULL;
    }

    sentence_count = get_paragraph_sentence_count((const char***)g_file_tokens[paragraph_index]);

    if (sentence_index >= sentence_count) {
        return NULL;
    }

    return (const char**)g_file_tokens[paragraph_index][sentence_index];
}

size_t get_sentence_word_count(const char** sentence)
{
    size_t result = 0;
    size_t i = 0;

    while (sentence[i] != NULL) {
        result++;
        i++;
    }

    return result;
}

int print_as_tree(const char* filename)
{
    FILE* stream;
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;

    if (g_file_contents == NULL || g_file_size == 0) {
        return FALSE;
    }

    stream = fopen(filename, "w");

    for (i = 0; i < get_total_paragraph_count(); i++) {
        if (i != 0) {
            fprintf(stream, "\n\n");
        }
        fprintf(stream, "Paragraph %d:", i);
        for (j = 0; j < get_paragraph_sentence_count(get_paragraph(i)); j++) {
            fprintf(stream, "\n    Sentence %d:", j);
            for (k = 0; k < get_sentence_word_count(get_sentence(i, j)); k++) {
                fprintf(stream, "\n        %s", g_file_tokens[i][j][k]);
            }
        }
    }

    fflush(stream);

    fclose(stream);

    return TRUE;
}


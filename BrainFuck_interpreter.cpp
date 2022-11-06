#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <conio.h>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstring>
using namespace std;

HANDLE hOut;
COORD cur;
CONSOLE_SCREEN_BUFFER_INFO Cur;

unsigned int Cwhle;
unsigned int backup;
vector<long> While;

char input;
char num;

char *buffer;
char *st_buf;

FILE* fp;

const int MAX_BUFFER = 4096;

bool run=true;

const COORD Screen = { 80,25 };

bool step,delay;


int main(int count,char *Strings[])
{
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleScreenBufferSize(hOut,Screen);

    if (count >= 2) {
        fp=fopen(Strings[1], "r");
        if (fp == NULL) {
            SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
            cout << "Can\'t find file[" << Strings[1] << "]";
            exit(1);
        }
        if (count == 3) {
            if (strcmp(Strings[2], "-debug") == 0) {
                step = true;
            }else if (strcmp(Strings[2], "-delay") == 0) {
                delay = true;
            }
        }

        buffer = (char*)calloc(MAX_BUFFER, sizeof(char));
        st_buf = buffer;
        cout << "Running... Memory[" << (int)(buffer - st_buf) << "/" << MAX_BUFFER << "]"<<endl;

        for (; fread(&input, 1, 1, fp)&&run;) {
            if (delay) {
                Sleep(80);
            }
            GetConsoleScreenBufferInfo(hOut,&Cur);
            cur.X = 0;
            cur.Y = 0;
            SetConsoleCursorPosition(hOut, cur);
            cout << "Running... Memory[" << (int)(buffer - st_buf) << "/" << MAX_BUFFER << "] ";
            cout << "Offset:" << ftell(fp) << " Command:" << input;
            SetConsoleCursorPosition(hOut,Cur.dwCursorPosition);

            switch (input)
            {
            case '>':
                if (buffer - st_buf >= MAX_BUFFER) {
                    run = false;
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED| FOREGROUND_INTENSITY);
                    cout <<endl<< "pointer error . \">\" "<<ftell(fp);
                    free(buffer);
                    std::fclose(fp);
                    exit(1);
                    break;
                }
                else {
                    buffer++;
                    break;
                }
            case '<':
                if (buffer == st_buf ) {
                    run = false;
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
                    cout <<endl<< "pointer error . \"<\" "<<ftell(fp);
                    free(buffer);
                    std::fclose(fp);
                    exit(1);
                    break;
                }
                else {
                    buffer--;
                    break;
                }
            case '+':
                *buffer = (*buffer == 127) ? -128 : *buffer + 1;
                break;
            case '-':
                *buffer = (*buffer == -128) ? 127 : *buffer - 1;
                break;
            case '.':
                cout << (char)*buffer;
                break;
            case ',':
                cin >> *buffer;
                break;
            case '[':
                if (*buffer!=0) {
                    Cwhle++;
                    While.push_back(ftell(fp));
                }
                else {
                    backup = Cwhle;
                    do {
                        num = (char)fread(&input, 1, 1, fp);
                        if (num == 0) {
                            SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
                            cout << endl << "while error : End Of file "<<ftell(fp);
                            free(buffer);
                            std::fclose(fp);
                            exit(1);
                            break;
                        }
                        if (input == '[') {
                            Cwhle++;
                        }
                        else if (input == ']') {
                            if (Cwhle == backup) {
                                break;
                            }
                            else {
                                Cwhle--;
                            }
                        }
                    } while (num);
                }
                break;
            case ']':
                
                if (Cwhle == 0) {
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
                    cout << endl << "while error : \"]\" "<<ftell(fp);
                    exit(1);
                }
                else
                {
                    fseek(fp,While[While.size()-1]-1, SEEK_SET);
                    While.pop_back();
                    Cwhle--;
                }
                break;
            default:
                break;
            }
            if (step) {
                while (!_kbhit());
                _getch();
            }
        }

        free(buffer); 

        std::fclose(fp);
    }

    CloseHandle(hOut);
}

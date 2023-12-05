#include "jjuggumi.h"
#include "keyin.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <Windows.h>
#include "canvas.h"
#pragma warning(disable:4996)

#define MAX_JEBIES 10

bool jebi_picking(bool lots[], int selected_lot, bool picked[]);
void jebi_shuffling(bool lots[], int n, bool picked[]);
void show_game(int round, int turn, int remaining_players, int selected_lot, bool picked[]);

int picking_status[PLAYER_MAX] = { 0 };
int selected_jebi = 0;
int turn = -1;
bool player[MAX_JEBIES] = { true }; // 플레이어 상태 배열
char dialog_text[50];

void jebi(void) {
    srand((unsigned int)time(NULL));
    bool jebies[MAX_JEBIES], picked[MAX_JEBIES] = { false };
    int round = 1;
    int n_alive = n_player; // 살아있는 플레이어 수 초기화
    key_t key;
    selected_jebi = 0;

    while (n_alive > 1) {
        for (int i = 0; i < n_alive; i++) {
            picked[i] = false;
        }
        jebi_shuffling(jebies, n_alive, picked);
        selected_jebi = 0;
        for (int i = 0; i < PLAYER_MAX; i++) {
            if (!player[i]) {
                picking_status[i] = 1;
            }
        }

        while (1) {
            // 다음 살아있는 플레이어 찾기
            for (int i = 0; i < PLAYER_MAX; i++) {
                if (player[i] && picking_status[i] == 0) {
                    turn = i;
                    break;
                }
            }
            show_game(round, turn, n_alive, selected_jebi, picked);

            // 키 입력 처리
            do {
                key = get_key();
                if (key == K_LEFT) {
                    do {
                        selected_jebi = (selected_jebi + n_alive - 1) % n_alive;
                    } while (picked[selected_jebi]);
                    show_game(round, turn, n_alive, selected_jebi, picked);
                }
                else if (key == K_RIGHT) {
                    do {
                        selected_jebi = (selected_jebi + 1) % n_alive;
                    } while (picked[selected_jebi]);
                    show_game(round, turn, n_alive, selected_jebi, picked);
                }
                else if (key == K_QUIT) {
                    printf("Quitting game...\n");
                    exit(1);
                }
            } while (key != K_SPACE);

            // 제비 선택 처리
            bool result = jebi_picking(jebies, selected_jebi, picked);
            if (result) {
                sprintf(dialog_text, "Player %d passed!\n", turn);
                picking_status[turn] = 1;
            }
            else {
                sprintf(dialog_text, "Player %d failed!\n", turn);
                player[turn] = false;
                n_alive--;
                for (int i = 0; i < PLAYER_MAX; i++) {
                    picking_status[i] = 0;
                }
                break; // 라운드 종료
            }

            // 자동으로 이미 선택된 다음의 제비로 옮기기
            do {
                selected_jebi = (selected_jebi + 1) % n_alive;
            } while (picked[selected_jebi] && n_alive > 1);
        }
        round++;
    }

    int last_standing;
    for (int i = 0; i < MAX_JEBIES; i++) {
        if (player[i] == true) {
            last_standing = i;
            break;
        }
    }
    printf("\nGame Over. Winner is player %d\n", last_standing);
}

bool jebi_picking(bool jebies[], int selected_jebies, bool picked[]) {
    picked[selected_jebies] = true;

    return jebies[selected_jebies];
}

void jebi_shuffling(bool jebies[], int n, bool picked[]) {
    for (int i = 0; i < n; i++) {
        if (!picked[i])
            jebies[i] = true;
    }
    int fail_jebi_index = rand() % n;
    while (picked[fail_jebi_index])
        fail_jebi_index = rand() % n;

    jebies[fail_jebi_index] = false;
}

void show_game(int round, int turn, int remaining_players, int selected_jebies, bool picked[]) {
    system("cls");
    printf("*****************************************\n");
    printf("*                                       *\n");
    printf("* ");
    for (int i = 0; i < MAX_JEBIES; i++) {
        if (i < remaining_players) {
            if (picked[i]) printf("  ");
            else if (i == selected_jebies) printf("@ ");
            else printf("? ");
        }
        else printf("  ");
    }
    printf("                  *\n");
    printf("*                                       *\n");
    printf("*****************************************\n");
    printf("round %d, turn: player %d\n", round, turn);
    printf("\n\n%s\n", dialog_text);
    gotoxy(N_ROW + 10, 0);
    print_status();

}
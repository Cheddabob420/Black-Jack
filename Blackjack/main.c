#include "blackjack.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  srand(time(NULL));
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("Unable to init SDL: %s", SDL_GetError());
    return 1;
  }
  if (TTF_Init() < 0) {
    SDL_Log("Failed to init SDL_ttf: %s", TTF_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Blackjack", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                        WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    SDL_Log("Failed to create window: %s", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    SDL_Log("Failed to create renderer: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  TTF_Font *font =
      TTF_OpenFont("/usr/share/fonts/liberation/LiberationSans-Bold.ttf", 18);
  if (!font) {
    SDL_Log("Failed to load font: %s", SDL_GetError());
  }

  Deck deck;
  init_deck(&deck);
  shuffle_deck(&deck);

  Hand player = {.count = 0};
  Hand dealer = {.count = 0};

  deal_cards(&deck, &player);
  deal_cards(&deck, &dealer);
  deal_cards(&deck, &player);
  deal_cards(&deck, &dealer);

  GameState state = STATE_PLAYER_TURN;
  char banner_msg[64] = "";
  bool running = true;
  SDL_Event event;

  if (calculate_hand_score(&player) == 21) {
    state = STATE_GAME_OVER;
    snprintf(banner_msg, sizeof(banner_msg), "PLAYER BLACKJACK!");
  }

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
          running = false;
        }
        if (state == STATE_PLAYER_TURN) {
          switch (event.key.keysym.scancode) {
          case SDL_SCANCODE_H:
            deal_cards(&deck, &player);
            if (calculate_hand_score(&player) > 21) {
              snprintf(banner_msg, sizeof(banner_msg), "BUST! DEALER WINS");
              state = STATE_GAME_OVER;
            }
            break;
          case SDL_SCANCODE_S:
            state = STATE_DEALER_TURN;
            break;
          default:
            break;
          }
        } else if (state == STATE_GAME_OVER) {
          if (event.key.keysym.scancode == SDL_SCANCODE_R) {
            if (deck.top > 40) {
              init_deck(&deck);
              shuffle_deck(&deck);
            }
            player.count = 0;
            dealer.count = 0;
            deal_cards(&deck, &player);
            deal_cards(&deck, &dealer);
            deal_cards(&deck, &player);
            deal_cards(&deck, &dealer);
            state = STATE_PLAYER_TURN;
            banner_msg[0] = '\0';

            if (calculate_hand_score(&player) == 21) {
              state = STATE_GAME_OVER;
              snprintf(banner_msg, sizeof(banner_msg), "PLAYER BLACKJACK!");
            }
          }
        }
      }
    }

    if (state == STATE_DEALER_TURN) {
      int dealer_score = calculate_hand_score(&dealer);
      while (dealer_score < 17) {
        deal_cards(&deck, &dealer);
        dealer_score = calculate_hand_score(&dealer);
      }

      int player_score = calculate_hand_score(&player);
      if (dealer_score > 21) {
        snprintf(banner_msg, sizeof(banner_msg), "DEALER BUSTS - YOU WIN!");
      } else if (player_score > dealer_score) {
        snprintf(banner_msg, sizeof(banner_msg), "YOU WIN!");
      } else if (player_score < dealer_score) {
        snprintf(banner_msg, sizeof(banner_msg), "DEALER WINS!");
      } else {
        snprintf(banner_msg, sizeof(banner_msg), "PUSH (TIE)!");
      }

      state = STATE_GAME_OVER;
    }

    // Render pass
    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
    SDL_RenderClear(renderer);

    bool hide_dealer_card = (state == STATE_PLAYER_TURN);
    render_hand(renderer, font, &dealer, 100, 50, hide_dealer_card);
    render_hand(renderer, font, &player, 100, 400, false);

    SDL_Color white = {255, 255, 255, 255};
    char score_text[64];

    int player_score = calculate_hand_score(&player);
    snprintf(score_text, sizeof(score_text), "Player Score: %d", player_score);
    render_text(renderer, font, score_text, 100, 515, white);

    if (hide_dealer_card) {
      render_text(renderer, font, "Dealer Score: ?", 100, 20, white);
    } else {
      int dealer_score = calculate_hand_score(&dealer);
      snprintf(score_text, sizeof(score_text), "Dealer Score: %d",
               dealer_score);
      render_text(renderer, font, score_text, 100, 20, white);
    }

    if (state == STATE_PLAYER_TURN) {
      render_text(renderer, font, "[H] Hit   [S] Stand   [ESC] Quit", 100, 560,
                  white);
    } else if (state == STATE_GAME_OVER) {
      render_text(renderer, font,
                  "ROUND OVER - Press [R] to Play Again, [ESC] to Quit", 100,
                  560, white);

      // Draw banner overlay
      SDL_Rect banner_rect = {.x = 200, .y = 220, .w = 400, .h = 100};
      SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
      SDL_RenderFillRect(renderer, &banner_rect);
      SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
      SDL_RenderDrawRect(renderer, &banner_rect);

      render_text(renderer, font, banner_msg, banner_rect.x + 40,
                  banner_rect.y + 20, white);
      render_text(renderer, font, "Press [R] to Play Again", banner_rect.x + 85,
                  banner_rect.y + 60, (SDL_Color){200, 200, 200, 255});
    }

    SDL_RenderPresent(renderer);
  }

  if (font)
    TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
  return EXIT_SUCCESS;
}

/* Includes */
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>

/* Defines */
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

/* Enums */
typedef enum {
  SUIT_HEARTS,
  SUIT_DIAMONDS,
  SUIT_CLUBS,
  SUIT_SPADES,
  SUIT_COUNT
} Suit;

typedef enum {
  RANK_2 = 2,
  RANK_3,
  RANK_4,
  RANK_5,
  RANK_6,
  RANK_7,
  RANK_8,
  RANK_9,
  RANK_10,
  RANK_JACK,
  RANK_QUEEN,
  RANK_KING,
  RANK_ACE
} Rank;

typedef enum {
  STATE_PLAYER_TURN,
  STATE_DEALER_TURN,
  STATE_GAME_OVER
} GameState;

/* Structs */
typedef struct {
  Suit suit;
  Rank rank;
  int value;
} Card;

typedef struct {
  Card cards[52];
  int top;
} Deck;

typedef struct {
  Card cards[12];
  int count;
} Hand;

/* Prototypes */
void init_deck(Deck *deck);
void shuffle_deck(Deck *deck);
void deal_cards(Deck *deck, Hand *hand);
int calculate_hand_score(Hand *hand);
void render_hand(SDL_Renderer *renderer, TTF_Font *font, Hand *hand,
                 int start_x, int start_y, bool hide_second_card);
void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                 int x, int y, SDL_Color color);

/* Main Logic */
int main(int argc, char *argv[]) {
  /* Setup */
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
    SDL_Log("Failed to load font: %s", TTF_GetError());
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

  printf("Player Score: %d\n", calculate_hand_score(&player));
  printf("Dealer Score: %d\n", calculate_hand_score(&dealer));

  GameState state = STATE_PLAYER_TURN;
  char banner_msg[64] = "";
  bool running = true;
  SDL_Event event;
  if (calculate_hand_score(&player) == 21) {
    state = STATE_GAME_OVER;
    snprintf(banner_msg, sizeof(banner_msg), "PLAYER BLACKJACK!");
    printf("Player has Blackjack!!\n");
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
            printf("Player Hits! New Score: %d\n",
                   calculate_hand_score(&player));
            if (calculate_hand_score(&player) > 21) {
              snprintf(banner_msg, sizeof(banner_msg), "BUST! DEALER WINS");
              printf("Player Busts! Dealer Wins!\n");
              state = STATE_GAME_OVER;
            }
            break;
          case SDL_SCANCODE_S:
            state = STATE_DEALER_TURN;
            printf("Player Stands. Dealer's Turn...\n");
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
            printf("\n--- NEW ROUND ---\n");
            printf("Player Score: %d\n", calculate_hand_score(&player));
            printf("Dealer Score: %d\n", calculate_hand_score(&dealer));

            if (calculate_hand_score(&player) == 21) {
              state = STATE_GAME_OVER;
              snprintf(banner_msg, sizeof(banner_msg), "PLAYER BLACKJACK!");
              printf("Player has Blackjack!\n");
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
        printf("Dealer hits. New Dealer Score: %d\n", dealer_score);
      }

      int player_score = calculate_hand_score(&player);
      printf("Final -> Player: %d | Dealer: %d\n", player_score, dealer_score);

      if (dealer_score > 21) {
        snprintf(banner_msg, sizeof(banner_msg), "DEALER BUSTS - YOU WIN!");
        printf("Dealer busts! Player wins!\n");
      } else if (player_score > dealer_score) {
        snprintf(banner_msg, sizeof(banner_msg), "YOU WIN!");
        printf("Player wins!\n");
      } else if (player_score < dealer_score) {
        snprintf(banner_msg, sizeof(banner_msg), "DEALER WINS!");
        printf("Dealer wins!\n");
      } else {
        snprintf(banner_msg, sizeof(banner_msg), "PUSH (TIE)!");
        printf("It's a push (tie)!\n");
      }

      state = STATE_GAME_OVER;
      printf("Press 'R' to play again, or 'ESC' to quit.\n");
    }

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
      SDL_Rect banner_rect = {.x = 200, .y = 220, .w = 400, .h = 100};

      // Draw solid dark background box for the banner
      SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
      SDL_RenderFillRect(renderer, &banner_rect);

      // Draw gold/white border around the banner
      SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
      SDL_RenderDrawRect(renderer, &banner_rect);

      // Render the outcome message centered inside the banner
      render_text(renderer, font, banner_msg, banner_rect.x + 40,
                  banner_rect.y + 20, (SDL_Color){255, 255, 255, 255});
      render_text(renderer, font, "Press [R] to Play Again, [ESC] to Quit",
                  banner_rect.x + 45, banner_rect.y + 60,
                  (SDL_Color){200, 200, 200, 255});
    }
    SDL_RenderPresent(renderer);
  }
  if (font)
    TTF_CloseFont(font);
  TTF_Quit();
  return EXIT_SUCCESS;
}

/* Functions */
void init_deck(Deck *deck) {
  int index = 0;
  for (int s = 0; s < SUIT_COUNT; s++) {
    for (int r = 2; r <= 14; r++) {
      deck->cards[index].suit = (Suit)s;
      deck->cards[index].rank = (Rank)r;
      if (r >= 2 && r <= 10) {
        deck->cards[index].value = r;
      } else if (r >= 11 && r <= 13) {
        deck->cards[index].value = 10;
      } else {
        deck->cards[index].value = 11;
      }
      index++;
    }
  }
  deck->top = 0;
}

void shuffle_deck(Deck *deck) {
  for (int i = 51; i > 0; i--) {
    int j = rand() % (i + 1);
    Card temp = deck->cards[i];
    deck->cards[i] = deck->cards[j];
    deck->cards[j] = temp;
  }
  deck->top = 0;
}

void deal_cards(Deck *deck, Hand *hand) {
  if (deck->top < 52 && hand->count < 12) {
    hand->cards[hand->count++] = deck->cards[deck->top++];
  }
}

int calculate_hand_score(Hand *hand) {
  int total = 0;
  int aces = 0;
  for (int i = 0; i < hand->count; i++) {
    total += hand->cards[i].value;
    if (hand->cards[i].rank == RANK_ACE) {
      aces++;
    }
  }
  while (total > 21 && aces > 0) {
    total -= 10;
    aces--;
  }
  return total;
}

void render_hand(SDL_Renderer *renderer, TTF_Font *font, Hand *hand,
                 int start_x, int start_y, bool hide_second_card) {
  int card_width = 70;
  int card_height = 100;
  int spacing = 20;

  for (int i = 0; i < hand->count; i++) {
    SDL_Rect card_rect = {.x = start_x + (i * (card_width + spacing)),
                          .y = start_y,
                          .w = card_width,
                          .h = card_height};

    if (hide_second_card && i == 1) {
      SDL_SetRenderDrawColor(renderer, 20, 40, 120, 255);
      SDL_RenderFillRect(renderer, &card_rect);
      SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
      SDL_RenderDrawRect(renderer, &card_rect);
      continue;
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &card_rect);
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderDrawRect(renderer, &card_rect);

    if (font) {
      Card *card = &hand->cards[i];
      char rank_str[3];

      switch (card->rank) {
      case RANK_JACK:
        snprintf(rank_str, sizeof(rank_str), "J");
        break;
      case RANK_QUEEN:
        snprintf(rank_str, sizeof(rank_str), "Q");
        break;
      case RANK_KING:
        snprintf(rank_str, sizeof(rank_str), "K");
        break;
      case RANK_ACE:
        snprintf(rank_str, sizeof(rank_str), "A");
        break;
      default:
        snprintf(rank_str, sizeof(rank_str), "%d", card->rank);
        break;
      }
      const char *suit_str;
      SDL_Color text_color = {0, 0, 0, 255}; // Default Black

      // Set suits and red color for Hearts/Diamonds
      switch (card->suit) {
      case SUIT_HEARTS:
        suit_str = "♥";
        text_color = (SDL_Color){200, 0, 0, 255};
        break;
      case SUIT_DIAMONDS:
        suit_str = "♦";
        text_color = (SDL_Color){200, 0, 0, 255};
        break;
      case SUIT_CLUBS:
        suit_str = "♣";
        break;
      case SUIT_SPADES:
        suit_str = "♠";
        break;
      default:
        suit_str = "";
        break;
      }

      char display_text[16];
      snprintf(display_text, sizeof(display_text), "%s %s", rank_str, suit_str);

      // Create text surface and texture
      SDL_Surface *text_surface =
          TTF_RenderUTF8_Blended(font, display_text, text_color);
      if (text_surface) {
        SDL_Texture *text_texture =
            SDL_CreateTextureFromSurface(renderer, text_surface);
        if (text_texture) {
          SDL_Rect text_rect = {.x = card_rect.x + 8,
                                .y = card_rect.y + 8,
                                .w = text_surface->w,
                                .h = text_surface->h};
          // Scale down text if it's slightly too wide for the card
          if (text_rect.w > card_width - 16) {
            text_rect.w = card_width - 16;
          }
          SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
          SDL_DestroyTexture(text_texture);
        }
        SDL_FreeSurface(text_surface);
      }
    }
  }
}

void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                 int x, int y, SDL_Color color) {
  if (!font)
    return;

  SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
  if (surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
      SDL_Rect rect = {.x = x, .y = y, .w = surface->w, .h = surface->h};
      SDL_RenderCopy(renderer, texture, NULL, &rect);
      SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
  }
}

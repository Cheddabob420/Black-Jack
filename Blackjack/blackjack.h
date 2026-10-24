#ifndef BLACKJACK_H
#define BLACKJACK_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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

void init_deck(Deck *deck);
void shuffle_deck(Deck *deck);
void deal_cards(Deck *deck, Hand *hand);
int calculate_hand_score(Hand *hand);

void render_hand(SDL_Renderer *renderer, TTF_Font *font, Hand *hand,
                 int start_x, int start_y, bool hide_second_card);
void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                 int x, int y, SDL_Color color);

#endif

#include "blackjack.h"
#include <stdio.h>

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
      SDL_Color text_color = {0, 0, 0, 255};

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

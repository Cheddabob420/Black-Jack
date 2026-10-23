#include "blackjack.h"
#include <stdlib.h>

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

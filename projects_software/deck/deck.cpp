#include <unordered_set>
#include <vector>
#include <random>
#include <iostream>

class Card {
public:
    enum class Suit {
        Clubs,
        Hears,
        Diamonds,
        Spades
    };
    enum class Value {
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Ten,
        Jack,
        Queen,
        King,
        Ace
    };

    static constexpr Suit AllSuits[] = {
        Suit::Clubs,
        Suit::Hears,
        Suit::Diamonds,
        Suit::Spades
    };
    static constexpr Value AllValues[] = {
        Value::Two,
        Value::Three,
        Value::Four,
        Value::Five,
        Value::Six,
        Value::Seven,
        Value::Eight,
        Value::Nine,
        Value::Ten,
        Value::Jack,
        Value::Queen,
        Value::King,
        Value::Ace
    };

    Card(Value v, Suit s) : value(v), suit(s)
    {}

    bool operator==(const Card &other) const
    {
        return (value == other.value && suit == other.suit);
    }

    std::string SuitStr() const
    {
        switch(suit) {
            case Card::Suit::Clubs:
                return u8"♣";
            case Card::Suit::Hears:
                return u8"♡";
            case Card::Suit::Diamonds:
                return u8"♢";
            case Card::Suit::Spades:
                return u8"♠";

            default:
                throw std::runtime_error("Unknown suit of the card");
        }
    }

    std::string ValueStr() const
    {
        switch(value) {
            case Card::Value::Two:
                return "Two";
            case Card::Value::Three:
                return "Three";
            case Card::Value::Four:
                return "Four";
            case Card::Value::Five:
                return "Five";
            case Card::Value::Six:
                return "Six";
            case Card::Value::Seven:
                return "Seven";
            case Card::Value::Eight:
                return "Eight";
            case Card::Value::Nine:
                return "Nine";
            case Card::Value::Ten:
                return "Ten";
            case Card::Value::Jack:
                return "Jack";
            case Card::Value::Queen:
                return "Queen";
            case Card::Value::King:
                return "King";
            case Card::Value::Ace:
                return "Ace";

            default:
                throw std::runtime_error("Unknown value of the card");
        }
    }

    Value value;
    Suit suit;
};

constexpr Card::Suit Card::AllSuits[];
constexpr Card::Value Card::AllValues[];

std::ostream& operator<<(std::ostream& os, const Card& card)
{
    os << card.ValueStr() << " of " << card.SuitStr();
    return os;
}

namespace std {
    template<>
    struct hash<Card>
    {
        size_t operator()(const Card& card) const
        {
            return hash<string>()(card.ValueStr()) ^ hash<string>()(card.SuitStr());
        }

    };
}


class Deck {
public:
    static const size_t kMaxCapacity = 52;
    friend std::ostream& operator<<(std::ostream& os, const Deck& deck);

    Deck(bool empty = false, size_t capacity = kMaxCapacity)
    {
        if (capacity > kMaxCapacity) {
            throw std::runtime_error("A Deck can contain " + std::to_string(kMaxCapacity) + " cards maximum");
        }

        capacity_ = capacity;
        if (!empty) {
            for (const Card::Value& v : Card::AllValues) {
                for (const Card::Suit& s : Card::AllSuits) {
                    PutCard(Card(v, s));
                }
            }
        }
    }

    Deck(const Deck& other) = default;
    Deck(Deck&& other) = default;

    void Shuffle()
    {
        size_t halfway = Capacity() / 2;
        for (size_t i = 0; i <= halfway; ++i)
        {
            PutCard(PopCard());
        }
    }

    Card PopCard()
    {
        if (Empty()) {
            throw std::runtime_error("The deck is empty");
        }

        auto deck_iter = deck_.begin();
        Card some_card = *deck_iter;
        deck_.erase(deck_iter);
        return some_card;
    }

    void PutCard(Card&& card)
    {
        if (Size() >= Capacity()) {
            throw std::runtime_error("The deck already contains " + std::to_string(Capacity()) + " cards");
        }

        deck_.insert(card);
    }

    Deck Split(size_t at_pos = 0)
    {
        while (at_pos == 0) {
            at_pos = GetRandomPosition();
        }

        Deck new_deck(true, Capacity());

        for (int i = 0; i < at_pos; ++i) {
            new_deck.PutCard(this->PopCard());
        }

        return new_deck;
    }

    void Merge(Deck&& from_deck)
    {
        if (Size() + from_deck.Size() > Capacity()) {
            throw std::runtime_error("Cumulative size of both deck exceeds " + std::to_string(Capacity()));
        }

        size_t from_size = from_deck.Size();
        for (int i = 0; i < from_size; ++i) {
            PutCard(from_deck.PopCard());
        }
    }

    size_t Size() const
    {
        return deck_.size();
    }

    size_t Capacity() const
    {
        return capacity_;
    }

    bool Empty() const
    {
        return Size() == 0;
    }

    bool operator==(const Deck &other) const
    {
        if (Capacity() != other.Capacity() ||
            Size() != other.Size())
        {
            return false;
        }

        return deck_ == other.deck_;
    }

    Deck& operator=(Deck&& other)
    {
        std::swap(deck_, other.deck_);
        return *this;
    }

    Deck& operator=(const Deck& other)
    {
        deck_ = other.deck_;
        return *this;
    }

private:
    size_t GetRandomPosition(size_t from = 0, size_t to = 0)
    {
        if (to == 0) {
            to = Size();
        }
        std::uniform_int_distribution<size_t> even_random(from, to);
        std::random_device random_generator_;
        return even_random(random_generator_);
    }

    std::unordered_set<Card> deck_;
    size_t capacity_;
};

std::ostream& operator<<(std::ostream& os, const Deck& deck)
{
    for (const Card& card : deck.deck_) {
        os << card << std::endl;
    }

    return os;
}



// class VectorDeck {
// public:
//     static const size_t kMaxCapacity = 52;
//     friend std::ostream& operator<<(std::ostream& os, const VectorDeck& deck);

//     VectorDeck(bool empty = false, size_t capacity = kMaxCapacity)
//     {
//         if (capacity > kMaxCapacity) {
//             throw std::runtime_error("A VectorDeck can contain " + std::to_string(kMaxCapacity) + " cards maximum");
//         }

//         deck_.reserve(capacity);
//         if (!empty) {
//             for (const Card::Value& v : Card::AllValues) {
//                 for (const Card::Suit& s : Card::AllSuits) {
//                     PutCard(Card(v, s));
//                 }
//             }
//         }
//     }

//     VectorDeck(const VectorDeck& other) = default;
//     VectorDeck(VectorDeck&& other) = default;

//     void Shuffle()
//     {
//         size_t halfway = Capacity() / 2;
//         for (size_t i = 0; i <= halfway; ++i)
//         {
//             size_t first_pos = GetRandomPosition(0, halfway);
//             size_t second_pos = GetRandomPosition(halfway, Capacity());
//             std::swap(deck_[first_pos], deck_[second_pos]);
//         }
//     }

//     Card PopCard()
//     {
//         if (Empty()) {
//             throw std::runtime_error("The deck is empty");
//         }

//         Card some_card = deck_.back();
//         deck_.pop_back();
//         return some_card;
//     }

//     void PutCard(Card&& card, bool random = false)
//     {
//         if (Size() >= Capacity()) {
//             throw std::runtime_error("The deck already contains " + std::to_string(Capacity()) + " cards");
//         }

//         if (!random) {
//             return deck_.push_back(card);
//         }

//         deck_.insert(deck_.begin() + GetRandomPosition(), card);
//     }

//     VectorDeck Split(size_t at_pos = 0)
//     {
//         while (at_pos == 0) {
//             at_pos = GetRandomPosition();
//         }

//         VectorDeck new_deck(true, Capacity());
//         std::move(deck_.begin() + at_pos, deck_.end(), std::back_inserter(new_deck.deck_));
//         deck_.erase(deck_.begin() + at_pos, deck_.end());
//         return new_deck;
//     }

//     void Merge(VectorDeck&& from_deck)
//     {
//         if (Size() + from_deck.Size() > Capacity()) {
//             throw std::runtime_error("Cumulative size of both deck exceeds " + std::to_string(Capacity()));
//         }

//         std::move(from_deck.deck_.begin(), from_deck.deck_.end(), std::back_inserter(deck_));
//         from_deck.deck_.erase(from_deck.deck_.begin(), from_deck.deck_.end());
//     }

//     size_t Size() const
//     {
//         return deck_.size();
//     }

//     size_t Capacity() const
//     {
//         return deck_.capacity();
//     }

//     bool Empty() const
//     {
//         return Size() == 0;
//     }

//     bool operator==(const VectorDeck &other) const
//     {
//         if (Capacity() != other.Capacity() ||
//             Size() != other.Size())
//         {
//             return false;
//         }

//         return deck_ == other.deck_;
//     }

//     VectorDeck& operator=(VectorDeck&& other)
//     {
//         std::swap(deck_, other.deck_);
//         return *this;
//     }

//     VectorDeck& operator=(const VectorDeck& other)
//     {
//         deck_ = other.deck_;
//         return *this;
//     }

// private:
//     size_t GetRandomPosition(size_t from = 0, size_t to = 0)
//     {
//         if (to == 0) {
//             to = Size();
//         }
//         std::uniform_int_distribution<size_t> even_random(from, to);
//         std::random_device random_generator_;
//         return even_random(random_generator_);
//     }

//     std::vector<Card> deck_;
// };




int main(){
    Deck card_deck;

    card_deck.Shuffle();
    // std::cout << card_deck;

    Deck tmp_deck = card_deck;

    std::cout << "Size = " << card_deck.Size() << " Capacity = " << card_deck.Capacity() << std::endl;

    Card poped_card = card_deck.PopCard();
    std::cout << "Popped card is " << poped_card << std::endl;
    std::cout << "Size = " << card_deck.Size() << " Capacity = " << card_deck.Capacity() << std::endl;

    Deck split_deck = card_deck.Split();
    std::cout << "After split size = " << card_deck.Size() << " Capacity = " << card_deck.Capacity() << std::endl;
    std::cout << "Splitted size = " << split_deck.Size() << " Capacity = " << split_deck.Capacity() << std::endl;

    card_deck.Merge(std::move(split_deck));
    std::cout << "After merge size = " << card_deck.Size() << " Capacity = " << card_deck.Capacity() << std::endl;
    std::cout << "Splitted size = " << split_deck.Size() << " Capacity = " << split_deck.Capacity() << std::endl;

    card_deck.PutCard(std::move(poped_card));
    // std::cout << card_deck;

    std::cout << "Equal - " << (tmp_deck == card_deck ? "true" : "false") << std::endl;
}

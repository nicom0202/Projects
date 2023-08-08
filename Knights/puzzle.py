from logic import *

AKnight = Symbol("A is a Knight")
AKnave = Symbol("A is a Knave")

BKnight = Symbol("B is a Knight")
BKnave = Symbol("B is a Knave")

CKnight = Symbol("C is a Knight")
CKnave = Symbol("C is a Knave")

# Puzzle 0
# A says "I am both a knight and a knave."
knowledge0 = And(
    # Rules: No person can be both a knight and knave, a knave always lies and a knight always tells the truth
    Or(AKnight,AKnave),
    Not(And(AKnight,AKnave)),

    # A says "I am both a knight and a knave."
    Implication(AKnight, And(AKnight, AKnave)),
    Implication(AKnave, Not(And(AKnight, AKnave)))
)

# Puzzle 1
# A says "We are both knaves."
# B says nothing.
knowledge1 = And(
    # Rules: No person can be both a knight and knave, a knave always lies and a knight always tells the truth
    Or(AKnight,AKnave),
    Or(BKnight,BKnight),
    Not(And(AKnight,AKnave)),
    Not(And(BKnight, BKnave)),

    # A says "We are both knaves."
    Implication(AKnight, And(AKnave, BKnave)),
    Implication(AKnave, Not(And(AKnave,BKnave)))

    # B says nothing.

)

# Puzzle 2
# A says "We are the same kind."
# B says "We are of different kinds."
knowledge2 = And(
    # Rules: No person can be both a knight and knave, a knave always lies and a knight always tells the truth
    Or(AKnight,AKnave),
    Or(BKnight,BKnave),
    Not(And(AKnight,AKnave)),
    Not(And(BKnight, BKnave)),

    # A says "We are the same kind."
    Implication(AKnight, And(AKnight, BKnight)),
    Implication(AKnight, And(AKnave, BKnave)),
    Implication(AKnave, Not(And(AKnight, BKnight))),
    Implication(AKnave, Not(And(AKnave, BKnave))),

    # B says "We are of different kinds."
    Implication(BKnight, Not(And(AKnight, BKnight))),
    Implication(BKnight, Not(And(AKnave, BKnave))),
    Implication(BKnave, And(AKnight, BKnight)),
    Implication(BKnave, And(AKnave, BKnave)),
)

# Puzzle 3
# A says either "I am a knight." or "I am a knave.", but you don't know which.
# B says "A said 'I am a knave'."
# B says "C is a knave."
# C says "A is a knight."
knowledge3 = And(
    
    # Rules: No person can be both a knight and knave, a knave always lies and a knight always tells the truth
    Or(AKnight, AKnave),
    Or(BKnight, BKnave),
    Or(CKnight, CKnave),
    Not(And(AKnight, AKnave)),
    Not(And(BKnight, BKnave)),
    Not(And(CKnight, CKnave)),

    # A says either "I am a knight." or "I am a knave.", but you don't know which.
    Implication(AKnight, Or(AKnight, AKnave)),
    Implication(AKnave, Not(Or(AKnight, AKnave))),

    # B says "A said 'I am a knave'."
    Or(Implication(BKnight, Or(Implication(AKnight,AKnave), Implication(AKnave, Not(AKnave)))), Implication(BKnave, Not(Or(Implication(AKnight,AKnave), Implication(AKnave, Not(AKnave)))))),

    # B says "C is a knave."
    Implication(BKnight, CKnave),
    Implication(BKnave, Not(CKnave)),

    # C says "A is a knight."
    Implication(CKnight, AKnight),
    Implication(CKnave, Not(AKnight))
)


def main():
    symbols = [AKnight, AKnave, BKnight, BKnave, CKnight, CKnave]
    puzzles = [
        ("Puzzle 0", knowledge0),
        ("Puzzle 1", knowledge1),
        ("Puzzle 2", knowledge2),
        ("Puzzle 3", knowledge3)
    ]
    for puzzle, knowledge in puzzles:
        print(puzzle)
        if len(knowledge.conjuncts) == 0:
            print("    Not yet implemented.")
        else:
            for symbol in symbols:
                if model_check(knowledge, symbol):
                    print(f"    {symbol}")


if __name__ == "__main__":
    main()

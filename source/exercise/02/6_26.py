"""
When a new gene is discovered, a standard approach to understanding its
function is to look through a database of known genes and find close matches.
The closeness of two genes is measured by the extent to which they are
aligned. To formalize this, think of a gene as being a long string over an
alphabet Σ = {A, C, G, T }.

Consider two genes (strings) x = ATGCC and y = TACGCA. An alignment of x and
y is a way of matching up these two strings by writing them in columns, for
instance:
    - A T - G C C
    T A - C G C A
Here the “−” indicates a “gap.” The characters of each string must appear in
order, and each column must contain a character from at least one of the
strings. The score of an alignment is specified by a scoring matrix δ of
size (|Σ| + 1) x (|Σ| + 1), where the extra row and column are to accommodate
gaps. For instance the preceding alignment has the following score:
    δ(-, T ) + δ(A, A) + δ(T, -) + δ(-, C) + δ(G, G) + δ(C, C) + δ(C, A).

Give a dynamic programming algorithm that takes as input two strings
x[1 ... n] and y[1 ... m] and a scoring matrix δ, and returns the
highest-scoring alignment. The running time should be O(mn).
"""

def sequence_alignment(scores: dict[tuple[str, str], int], x: str, y: str) -> tuple[str, str]:
    """
    Calculates the best alignment of the given strings.

    :param scores: The matrix containing the scores for each sequence pair.
    :param x: The first string that should be aligned.
    :param y: The second string that should be aligned.
    :return: The aligned versions of the given strings.
    """
    m, n = len(x), len(y)
    df = [[-100000] * (n + 1) for _ in range(m + 1)]
    
    df[0][0] = 0
    for i in range(m):
        df[i + 1][0] = df[i][0] + scores[(x[i], '-')]
    for j in range(n):
        df[0][j + 1] = df[0][j] + scores[('-', y[j])]

    for i in range(m):
        for j in range(n):
            df[i + 1][j + 1] = max(
                df[i][j] + scores[(x[i], y[j])],
                df[i][j + 1] + scores[(x[i], '-')],
                df[i + 1][j] + scores[('-', y[j])]
            )

    x_align = ""
    y_align = ""
    i = m
    j = n
    while i > 0 and j > 0:
        if df[i][j] == df[i-1][j-1] + scores[(x[i-1], y[j-1])]:
            x_align = x[i-1] + x_align
            y_align = y[j-1] + y_align
            i -= 1
            j -= 1
        elif df[i][j] == df[i-1][j] + scores[(x[i-1], '-')]:
            x_align = x[i-1] + x_align
            y_align = '-' + y_align
            i -= 1
        else:
            x_align = '-' + x_align
            y_align = y[j-1] + y_align
            j -= 1

    x_align = '-' * j + x[:i] + x_align
    y_align = '-' * i + y[:j] + y_align
    return x_align, y_align


if __name__ == "__main__":
    scores = {
        ('-', '-'): -1000,
        ('-', 'A'): -1,
        ('-', 'C'): -1,
        ('-', 'G'): -1,
        ('-', 'T'): -1,

        ('A', '-'): -1,
        ('A', 'A'): 20,
        ('A', 'C'): -5,
        ('A', 'G'): -5,
        ('A', 'T'): -5,

        ('C', '-'): -1,
        ('C', 'A'): -5,
        ('C', 'C'): 20,
        ('C', 'G'): -5,
        ('C', 'T'): -5,

        ('G', '-'): -1,
        ('G', 'A'): -5,
        ('G', 'C'): -5,
        ('G', 'G'): 20,
        ('G', 'T'): -5,

        ('T', '-'): -1,
        ('T', 'A'): -5,
        ('T', 'C'): -5,
        ('T', 'G'): -5,
        ('T', 'T'): 20,
    }

    x = "ATGCC"
    y = "TACGCA"

    x_align, y_align = sequence_alignment(scores, x, y)
    print("Best alignment: ")
    print(' '.join(x_align))
    print(' '.join(y_align))
    assert(x_align == "-A-TGC-C")
    assert(y_align == "TAC-GCA-")

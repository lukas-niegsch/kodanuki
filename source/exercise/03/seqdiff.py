import random
import string
from dataclasses import dataclass
from enum import Enum
from typing import *

T = TypeVar('T')

class EditOpType(Enum):
    Insert = 'Insert'
    Delete = 'Delete'
    Replace = 'Replace'

@dataclass
class EditOp:
    type: EditOpType
    index: int
    value: Optional[T]

def seqdiff(fst: Sequence[T], snd: Sequence[T]) -> list[EditOp]:
    """
    Computes the operations responsable for the Levenshtein distance.

    We use Hirschberg's trick:
    - use bottom up dynamic programming on two halves of the matrix
      to compute the edit distance between them
    - find the index at which the edit occurs
    - split the problem into two subproblems at the given index
    - combine all suboperations derived recursively

    The complexity is O(n * m) since we have the recursive formula:
    - T(n, m) = 2 * T(n / 2, m / 2) + O(n * m)

    :param fst: The starting sequence for the edit distance.
    :param snd: The ending sequence for the edit distance.
    :return: The minmal operations required to convert fst to snd.
    """
    if not fst:
        return [EditOp(EditOpType.Insert, i, v) for i, v in enumerate(snd)]
    if not snd:
        return [EditOp(EditOpType.Delete, 0, None) for i in range(len(fst))]
    if len(fst) == 1:
        return insert_except_common(fst, snd) # all different -> replace one
    if len(snd) == 1:
        return delete_except_common(fst, snd) # all different -> replace one
    mid_fst = len(fst) // 2 
    row_fwd = compute_levenshtein_distance(fst[:mid_fst], snd)
    row_bwd = compute_levenshtein_distance(fst[mid_fst:][::-1], snd[::-1])
    mid_snd = argmin(map(sum, zip(row_fwd, reversed(row_bwd))))
    ops1 = seqdiff(fst[:mid_fst], snd[:mid_snd])
    ops2 = seqdiff(fst[mid_fst:], snd[mid_snd:])
    inserts = sum(1 for op in ops1 if op.type == EditOpType.Insert)
    deletes = sum(1 for op in ops1 if op.type == EditOpType.Delete)
    offset = inserts - deletes
    ops2 = [EditOp(op.type, op.index + mid_fst + offset, op.value) for op in ops2]
    return ops1 + ops2

def argmin(iterable):
    """
    Computes the index of the minimum element for the sequence.

    :param iterable: The inite sequence to find the minimum.
    :return: The fist index at which the minimum is reached.
    """
    return min(enumerate(iterable), key=lambda x: x[1])[0]

def compute_levenshtein_distance(fst: Sequence[T], snd: Sequence[T]) -> list[int]:
    """
    Computes the last row for the bottom up dynamic programming table
    for the levenshtein distance.

    :param fst: The starting sequence for the edit distance.
    :param snd: The ending sequence for the edit distance.
    :return: The last row for the edit distance calculation.
    """
    prev_row = [i for i in range(len(snd) + 1)]
    curr_row = [0 for _ in range(len(snd) + 1)]

    for i in range(len(fst)):
        curr_row[0] = i + 1
        for j in range(len(snd)):
            if fst[i] == snd[j]:
                curr_row[j + 1] = prev_row[j]
            else:
                curr_row[j + 1] = 1 + min(prev_row[j], prev_row[j + 1], curr_row[j])
        prev_row = curr_row[:]
    return curr_row

def insert_except_common(fst: str, snd: str) -> list[EditOp]:
    """
    Makes edit operations that insert the snd string into the fst one.
    Skips one common character or replaces the first if there is none.

    :param fst: The starting sequence for the edit distance.
    :param snd: The ending sequence for the edit distance.
    :return: The minmal operations required to convert fst to snd.
    """
    assert(len(fst) == 1)
    if not fst[0] in snd:
        ops2 = seqdiff(None, snd[1:])
        ops2 = [EditOp(op.type, op.index + 1, op.value) for op in ops2]
        return [EditOp(EditOpType.Replace, 0, snd[0])] + ops2
    else:
        idx = snd.find(fst[0])
        ops1 = [EditOp(EditOpType.Insert, i, c) for i, c in enumerate(snd) if i != idx]
        return ops1

def delete_except_common(fst: str, snd: str) -> list[EditOp]:
    """
    Makes edit operations that delete the fst string into the snd one.
    Skips one common character or replaces the first if there is none.

    :param fst: The starting sequence for the edit distance.
    :param snd: The ending sequence for the edit distance.
    :return: The minmal operations required to convert fst to snd.
    """
    assert(len(snd) == 1)
    if not snd[0] in fst:
        ops2 = seqdiff(fst[1:], None)
        ops2 = [EditOp(op.type, op.index + 1, op.value) for op in ops2]
        return [EditOp(EditOpType.Replace, 0, snd[0])] + ops2
    else:
        idx = fst.find(snd[0])
        ops1 = [EditOp(EditOpType.Delete, 0 + int(i > idx), c) for i, c in enumerate(fst) if i != idx]
        return ops1

if __name__ == '__main__':
    verbose = False
    #sample_fst = ['nematode-knowledge', 'nematode-', 'nema', 'ne', 'ma', 'm', 'a', '', 'tode-', 'to', 'de-', 'd', 'e-', 'e', '', '-', 'knowledge', 'know', 'kn', 'n', 'ow', 'ledge', 'le', 'e', 'dge', 'd', '', 'ge', 'g', '', 'e']
    #sample_snd = ['empty-bottle', 'empty-', 'emp', 'e', 'mp', 'm', 'p', '', 'ty-', 't', 'y-', '', 'y-', 'y', '', '-', 'bottle', 'bo', 'b', '', 'o', 'ttle', 't', '', 'tle', 't', '', 'le', 'l', '', 'e']
    sample_fst = [''.join(random.choices(string.ascii_uppercase, k = 500)) for _ in range(50)]
    sample_snd = [''.join(random.choices(string.ascii_uppercase, k = 500)) for _ in range(50)]
    # sample_fst = ['nema']
    # sample_snd = ['emp']

    sample_correct = []
    sample_wrong = []
    
    for x_orig, y_orig in zip(sample_fst, sample_snd):
        x = x_orig
        y = y_orig
        ops = seqdiff(x, y)
        for op in ops:
            if verbose:
                print(op, x)
            if op.type == EditOpType.Insert:
                x = x[:op.index] + op.value + x[op.index:]
            elif op.type == EditOpType.Delete:
                x = x[:op.index] + x[op.index + 1:]
            else:
                x = x[:op.index] + op.value + x[op.index + 1:]
        if verbose:
            print(f"{x} == {y} ?")
        if x != y:
            sample_wrong.append(f"{x_orig} -> {x} is not {y}")
        else:
            sample_correct.append(f"{x_orig} -> {x} is correct")
    
    print(f"Correct: {len(sample_correct)}")
    print(f"Wrong: {len(sample_wrong)}")
    # for sample in sample_correct:
    #     print(sample)
    for sample in sample_wrong:
        print(sample)

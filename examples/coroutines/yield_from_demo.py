def count(n: int):
    for i in range(1, n + 1):
        yield i


def gencat(g1, g2):
    yield from g1
    yield from g2
    #for thing in g1:
    #    yield thing
    #for thing in g2:
    #    yield thing


print(list(count(5)))
print(list(gencat(count(3), count(3))))

#!/usr/bin/env python

# coroutine demo (yields values _and_ receives "sent" values back)
def squarez():
    x = yield   # yield no value (None), but wait for a "sent" value back
    sofar = 0
    while x is not None:
        sofar += x**2
        print(f"{x}**2 == {x**2} ({sofar} so far)")
        x = yield x**2 # yield the square of the last sent-value; wait for next
    return sofar


def main():
    sq = squarez()
    sq.send(None) # "prime" the coroutine to the first yield

    # send 0..9 to the coroutine in sequence
    for i in range(10):
        print("coroutine yielded ->",sq.send(i))

    print('(sending "end" signal to coroutine)')
    try:
        sq.send(None) # give coroutine the "end" signal
    except StopIteration as stop:
        print("coroutine returned ->", stop.value)


if __name__ == "__main__":
    main()

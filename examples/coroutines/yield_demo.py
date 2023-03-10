import random

def produce_randnums_eager():
    nums = []
    for i in range(5):
        num = random.randrange(100)
        nums.append(num)

    return nums

def produce_randnums_lazy():
    for i in range(5):
        num = random.randrange(100)
        print('Before yield... num = ',num)
        yield num
        print('After yield... num = ',num)

def main():
    for number in produce_randnums_eager():
        print(number)
    for number in produce_randnums_lazy():
        print(number)

main()

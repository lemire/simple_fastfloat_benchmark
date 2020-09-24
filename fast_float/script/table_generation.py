
def format(number):
    # move the most significant bit in position
    while(number < (1<<127)):
        number *= 2
    # then *truncate*
    while(number >= (1<<128)):
        number //= 2
    upper = number // (1<<64)
    lower = number % (1<<64)
    print(""+hex(upper)+","+hex(lower)+",")

for q in range(-344,0):
    power5 = 5 ** -q
    z = 0
    while( (1<<z) < power5) :
        z += 1
    format(2 **(2 * z + 128) // power5)
for q in range(0,308+1):
    power5 = 5 ** q
    format(power5)

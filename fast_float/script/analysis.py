from math import floor

def log2(x):
   y = 0
   while((1<<y) < x):
     y = y + 1
   return y

maxval = 9999999999999999999*2**64

for q in range(1,1000):
    d = 5**q
    b = 127 + log2(d)
    t = 2** b
    N = maxval
    c = t//d + 1
    assert c < 2**128
    assert c >= 2**127
    K = N - (N%d)
    if(not(c * K * d<=( K + 1) * t)):
      print(q)
      #print (d * c * K < t * (K + 1) )
      top = floor(t/(c  * d - t))
      if(maxval > top):
         print(hex(floor(t/(c  * d - t))), hex(N))


   #c  * d /t<=1-1/K
   #1/K<=1-c  * d /t
   #K>=1/(1-c  * d /t)

# importing the required module 
import timeit 

def usleep(delay):
   mdelay = delay /1000
   now = time.time()
   while now + mdelay > time.time():
      pass

# code snippet to be executed only once
setup = "from time import sleep"

# code snippet whose execution time is to be measured
code = ''' 
def example(): 
    usleep(50)
'''

# timeit statement
print(timeit.timeit(setup=setup, stmt=code, number=10000))

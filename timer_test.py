# importing the required module 
import timeit 

# code snippet to be executed only once
setup = "from time import sleep"

# code snippet whose execution time is to be measured
code = ''' 
def example(): 
    sleep(1.0/1000.0)
'''

# timeit statement
print(timeit.timeit(setup=setup, stmt=code, number=10000))

from setuptools import setup, find_packages

setup(name='MotorLibrary',
      version='0.1',
      description='Library for Motor Driver Cape',
      long_description='SW for use with I19 motor driver board on the pocketBeagle.',
      classifiers=[],
      keywords='motor driver robot cape beagle',
      url='',
      author='Mitchell Whitlatch',
      author_email='robert.whitlatch@utexas.edu',
      license='MIT',
      packages=find_packages(),
      install_requires=[
          'Adafruit_GPIO',
      ],
      include_package_data=False,
      zip_safe=False)

from setuptools import setup, find_packages
import os
from glob import glob

package_name = 'pct_planner'

setup(
    name=package_name,
    version='2.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'config'), glob('config/*.py')),
    ],
    install_requires=['setuptools', 'numpy', 'scipy', 'cupy-cuda12x'],
    zip_safe=True,
    maintainer='3d-navi',
    maintainer_email='dev@3dnavi.dev',
    description='PCT Planner ROS2 port',
    license='MIT',
    entry_points={
        'console_scripts': [
            'plan_node.py = pct_planner.plan_node:main',
            'tomography_node.py = pct_planner.tomography_node:main',
        ],
    },
)

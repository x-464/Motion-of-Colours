A C++/python project inspired by the Motion of Colours

The 'video.mp4' file is a demo file created for this project, if you want to use your own file, anything will work as long as it's an mp4.
Be aware that this is not fast, it takes a while to get and calculate all frame data.
This is unavoidable in Python and currently in C++, however with Vulkan the C++ speeds could be significantly improved.

Python: I used openCV to get video data and then read individual colours, numpy then calculates the averages and tkinter is used to display the final image.
It's all been commented and can probably be improved in the future, although not as greatly as the C++ version.

C++: Also used openCV to get video and frame data but everything else (like calculations and window creation) can be done in base C++.
The C++ project has the greatest potential to get much faster, using Vulkan to do calculations for colour averaging on the GPU would 
significantly improve speeds as the CPU is not meant for tasks like this. 
Vulkan is hard.

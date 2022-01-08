g++ deps/stb_image.cpp deps/stb_image_write.cpp deps/tiny_obj_loader.cpp -Ideps include/arc.cpp -Iinclude \
  -O2 -fopenmp -std=c++17 -o build/$1 samples/$1/$1.cpp

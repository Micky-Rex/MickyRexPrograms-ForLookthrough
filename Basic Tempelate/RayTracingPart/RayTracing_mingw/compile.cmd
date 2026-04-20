g++ -std=c++14 -fopenmp ./*.cpp -I %INCLUDE% -L %LIB%/SFML_MinGW64 -lsfml-system -lsfml-window -lsfml-graphics -o ray_tracing
pause
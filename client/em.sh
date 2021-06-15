
#./emsdk install latest
#./emsdk activate latest
#source ./emsdk_env.sh

#emcc -Iem-include proteo_opencv.c em-lib/liblua.a -o proteo_opencv.html --shell-file ./proteo_shell_minimal.html

if [[ $1 == DEBUG ]]; then
	echo "DEBUG MODE"
	#emcc -Iem-include proteo_opencv.cpp  em-lib/liblua.a em-lib/libopencv_core.a -lidbfs.js -O2 -g1 -s LLD_REPORT_UNDEFINED -s ASSERTIONS=2 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=2 -s WASM=1 -s FETCH=1 
	emcc -Iem-include  proteo_opencv.cpp proteo.c em-lib/liblua.a em-lib/libopencv_*.a -lidbfs.js -O2 -g1 -s LLD_REPORT_UNDEFINED -s ASSERTIONS=2 -s STACK_OVERFLOW_CHECK=2 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp","png"]' -s USE_SDL_TTF=2 -s USE_SDL_GFX=2 -s FETCH=1 -s EXPORTED_FUNCTIONS='["_main","_em_login_function","_em_header_callback","_proteo_get_callback","_proteo_post_callback"]' -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -o index.html --shell-file ./proteo_shell_minimal.html
elif [[ $1 == SAN ]]; then
	echo "SANITIZER MODE"
	emcc -Iem-include proteo_opencv.cpp proteo.c em-lib/liblua.a em-lib/libopencv_*.a -lidbfs.js -O2 -g1 -fsanitize=address -s ALLOW_MEMORY_GROWTH -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp","png"]' -s USE_SDL_TTF=2 -s USE_SDL_GFX=2 -s FETCH=1 -s EXPORTED_FUNCTIONS='["_main","_em_login_function","_em_header_callback","_proteo_get_callback","_proteo_post_callback"]' -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -o index.html --shell-file ./proteo_shell_minimal.html
elif [[ $1 == NOOPENCV ]]; then
	echo "NO OPENCV MODE"
	emcc -Iem-include  proteo.c em-lib/liblua.a -lidbfs.js -Oz -fsanitize=undefined -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 -g1 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp","png"]' -s USE_SDL_TTF=2 -s USE_SDL_GFX=2 -s FETCH=1 -s EXPORTED_FUNCTIONS='["_main","_em_login_function","_em_header_callback","_proteo_get_callback","_proteo_post_callback","_image_to_mat"]' -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -s NO_DISABLE_EXCEPTION_CATCHING -o index.html --shell-file ./proteo_shell_minimal.html
else
	emcc -Iem-include  proteo_opencv.cpp proteo.c em-lib/liblua.a em-lib/libopencv_*.a -lidbfs.js -Oz -fsanitize=undefined -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 -g1 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp","png"]' -s USE_SDL_TTF=2 -s USE_SDL_GFX=2 -s FETCH=1 -s EXPORTED_FUNCTIONS='["_main","_em_login_function","_em_header_callback","_proteo_get_callback","_proteo_post_callback","_image_to_mat"]' -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -s NO_DISABLE_EXCEPTION_CATCHING -o index.html --shell-file ./proteo_shell_minimal.html
fi


#emcc -Iem-include proteo_opencv.c em-lib/liblua.a -o proteo_opencv.html --shell-file ./proteo_shell_minimal.html

if [[ $1 == DEBUG ]]; then
	echo "DEBUG MODE"
	emcc -Iem-include proteo.c em-lib/liblua.a -lidbfs.js -O2 -g1 -s ASSERTIONS=2 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=2 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp","png"]' -s USE_SDL_TTF=2 -s USE_SDL_GFX=2 -s FETCH=1 -s EXPORTED_FUNCTIONS='["_main","_em_login_function","_em_header_callback","_proteo_get_callback","_proteo_post_callback"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -o index.html --shell-file ./proteo_shell_minimal.html
elif [[ $1 == SAN ]]; then
	echo "SANITIZER MODE"
	emcc -Iem-include proteo.c em-lib/liblua.a -lidbfs.js -O2 -g1 -fsanitize=address -s ALLOW_MEMORY_GROWTH -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp","png"]' -s USE_SDL_TTF=2 -s USE_SDL_GFX=2 -s FETCH=1 -s EXPORTED_FUNCTIONS='["_main","_em_login_function","_em_header_callback","_proteo_get_callback","_proteo_post_callback"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -o index.html --shell-file ./proteo_shell_minimal.html
else
	emcc -Iem-include  proteo.c em-lib/liblua.a -lidbfs.js -Oz -s ASSERTIONS=1 -g1 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp","png"]' -s USE_SDL_TTF=2 -s USE_SDL_GFX=2 -s FETCH=1 -s EXPORTED_FUNCTIONS='["_main","_em_login_function","_em_header_callback","_proteo_get_callback","_proteo_post_callback"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -o index.html --shell-file ./proteo_shell_minimal.html
fi

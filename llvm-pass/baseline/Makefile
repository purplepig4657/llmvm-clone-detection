compiler = ${CXX}  # clang++
llvm_config = ${LLVM_CONFIG}  # llvm-config

LLVM_FLAGS := `$(llvm_config) --cppflags --ldflags`
LLVM_LIBS := `$(llvm_config) --system-libs --libs all`

.PHONY: clean

all:
	$(compiler) -std=c++17 -fPIC -shared -O0 -g $(LLVM_FLAGS) $(LLVM_LIBS) transform.cpp -o transform.so
clean:
	rm -rf transform.so


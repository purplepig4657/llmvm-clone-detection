import re
import subprocess
from typing import Optional


class LLVMCompiler():
    def __init__(self, compiler: Optional[str] = None, prefix: Optional[str] = None):
        if compiler is None:
            self.compiler = "${CXX}"
        else:
            self.compiler = compiler
        
        if prefix is None:
            self.prefix = "#include <iostream>\n#include <cmath>\n#include <cstring>\nusing namespace std;\n"
        else:
            self.prefix = prefix 

    def get_llvm_ir(self, code: str) -> str:
        code = re.sub(r'^main', 'int main', code, flags=re.MULTILINE)
        code = re.sub(r'^void main', 'int main', code, flags=re.MULTILINE)
        code = re.sub(r'\\n', '', code)
        code = re.sub(r'\"', '\\\"', code)
        try:
            output = subprocess.check_output(f"echo \"{self.prefix}{code}\" | {self.compiler} -emit-llvm -S -x c++ - -o - 2> /dev/null", shell=True)
        except subprocess.CalledProcessError as _:
            return "Error"
        return output.decode('UTF-8')


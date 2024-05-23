import ctypes

from typing import Optional

class Transform():
    def __init__(
            self, 
            pass_path: str = "../../llvm-pass/baseline/transform.so", 
        ):
        self.pass_path = pass_path

        self.lib = ctypes.CDLL(pass_path)
        self.lib.transform.argtypes = [ctypes.c_char_p]
        self.lib.transform.restype = ctypes.c_void_p

        self.lib.myFree.argtypes = [ctypes.c_void_p]
        self.lib.myFree.restype = None
 
    def transform(self, llvm_ir: Optional[str]):
        if llvm_ir == "Error":
            return "Error"
        c_string = ctypes.c_char_p(llvm_ir.encode('UTF-8'))
        output = self.lib.transform(c_string)
        result = ctypes.cast(output, ctypes.c_char_p).value 
        self.lib.myFree(output)
        return result.decode('UTF-8')


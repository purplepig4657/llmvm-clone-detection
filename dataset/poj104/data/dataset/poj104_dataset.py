import pandas as pd

from datasets import load_dataset
from torch.utils.data import Dataset

from data.transform import LLVMCompiler, Transform


class POJ104LLVMDataset(Dataset):
    def __init__(self):
        self.dataset = load_dataset('google/code_x_glue_cc_clone_detection_poj104', split='train')
        self.item = pd.DataFrame(self.dataset)
        self.llvm_compiler = LLVMCompiler()

    def __getitem__(self, idx):
        return {
            'id': self.item['id'][idx],
            'label': self.item['label'][idx],
            'code': self.llvm_compiler.get_llvm_ir(self.item['code'][idx]),
        }
    
    def __len__(self):
        return len(self.item)


class POJ104TransformedDataset(Dataset):
    def __init__(self, item):
        self.item = item
        self.transform = Transform()

    def __getitem__(self, idx):
        return {
            'id': self.item['id'][idx],
            'label': self.item['label'][idx],
            'code': self.transform.transform(self.item['code'][idx]),
        }

    def __len__(self):
        return len(self.item)


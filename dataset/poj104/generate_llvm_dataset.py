import csv
import sys

from data.dataset.poj104_dataset import POJ104LLVMDataset

error = 0
success = 0

dataset = POJ104LLVMDataset()
with open('poj104_llvm.csv', 'w') as f:
    w = csv.writer(f)
    w.writerow(['id', 'label', 'code'])

    for i in range(len(dataset)):
        data = dataset[i]

        if data['code'] == "Error":
            error += 1
            print('@', end='')
        else:
            success += 1
            print('.', end='')

        w.writerow(data.values())
        sys.stdout.flush()  # Ensure the output is flushed immediately

print()
print(f'Errors: {error}')
print(f'Successes: {success}')

